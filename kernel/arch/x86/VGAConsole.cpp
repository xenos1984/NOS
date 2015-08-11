// VGAConsole.cpp - VGA console driver for the kernel.

#include INC_ARCH(VGAConsole.h)
#include <Symbol.h>
#include INC_ARCH(Port.h)

using namespace Kernel;

SECTION(".init.text") VGAConsole::VGAConsole(void)
{
	attrib = 0x07;
	numLines = 25;
	numColumns = 80;
	xPos = yPos = 0;

	videoMemory = (volatile unsigned char*)(COLOR_VIDEO + Symbol::kernelOffset.Addr());
	bytesPerLine = numColumns * 2;
	totalBytes = 4000;
}

VGAConsole::~VGAConsole(void)
{
}

void VGAConsole::Clear(void)
{
	int lengthInDWords = totalBytes / 4;
	int pattern = (attrib << 24) | (attrib << 8);

	while(lengthInDWords--)
		((int*)(videoMemory))[lengthInDWords] = pattern;
	xPos = yPos = 0;
}

uint8_t VGAConsole::GetBackground(void)
{
	return (attrib & 0x70);
}

void VGAConsole::SetBackground(uint8_t value)
{
	attrib &= 0x8f;
	attrib |= value;
}

uint8_t VGAConsole::GetForeground(void)
{
	return (attrib & 0x0f);
}

void VGAConsole::SetForeground(uint8_t value)
{
	attrib &= 0xf0;
	attrib |= value;
}

// Put the character C on the screen.
void VGAConsole::putChar(unsigned char c)
{
	char *f, *t;
	unsigned int i;

	Port::WriteU8(DebugPort, c); // Bochs debugger console

	if(c == '\t')
	{
		xPos += (8 - (xPos & 7));
		if(xPos < numColumns)
			return;
	}
	else if(c != '\n' && c != '\r')
	{
		*(videoMemory + (xPos + yPos * numColumns) * 2) = c;
		*(videoMemory + (xPos + yPos * numColumns) * 2 + 1) = attrib;

		xPos++;
		if(xPos < numColumns)
			return;
	}

	xPos = 0;
	yPos++;
	if(yPos >= numLines)
	{
		yPos = numLines - 1;
		// Scroll up a line.
		t = (char*)videoMemory;
		f = (char*)(videoMemory + bytesPerLine);
		for(i = 0; i < totalBytes - bytesPerLine; i++)
			t[i] = f[i];
		// Clear the last line.
		for(; i < totalBytes; i++)
			t[i] = 0;
	}
}

void VGAConsole::moveCursor(void)
{
	uint16_t chrOffset = xPos + yPos * numColumns;

	Port::WriteU8(CrtcAddr, 14);
	Port::WriteU8(CrtcData, chrOffset >> 8);
	Port::WriteU8(CrtcAddr, 15);
	Port::WriteU8(CrtcData, chrOffset);
}

char console_space[sizeof(VGAConsole)];
