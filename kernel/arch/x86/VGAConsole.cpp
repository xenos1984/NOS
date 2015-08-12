// VGAConsole.cpp - VGA console driver for the kernel.

#include INC_ARCH(VGAConsole.h)
#include <Symbol.h>
#include INC_ARCH(Port.h)

using namespace Kernel;

SECTION(".init.text") VGAConsole::VGAConsole(void)
{
	attrib = 0x07;
	numLines = lines;
	numColumns = columns;
	xPos = yPos = 0;

	videoMemory = (volatile unsigned char*)(COLOR_VIDEO + Symbol::kernelOffset.Addr());
}

VGAConsole::~VGAConsole(void)
{
}

void VGAConsole::Clear(void)
{
	int length = total / sizeof(long);
	unsigned long pattern = clearpat();

	while(length--)
		((unsigned long*)(videoMemory))[length] = pattern;
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
		if(xPos < columns)
			return;
	}
	else if(c != '\n' && c != '\r')
	{
		*(videoMemory + (xPos + yPos * columns) * 2) = c;
		*(videoMemory + (xPos + yPos * columns) * 2 + 1) = attrib;

		xPos++;
		if(xPos < columns)
			return;
	}

	xPos = 0;
	yPos++;
	if(yPos >= lines)
	{
		yPos = lines - 1;
		// Scroll up a line.
		t = (char*)videoMemory;
		f = (char*)(videoMemory + bpl);
		for(i = 0; i < total - bpl; i++)
			t[i] = f[i];
		// Clear the last line.
		for(; i < total; i++)
			t[i] = 0;
	}
}

void VGAConsole::moveCursor(void)
{
	uint16_t chrOffset = xPos + yPos * columns;

	Port::WriteU8(CrtcAddr, 14);
	Port::WriteU8(CrtcData, chrOffset >> 8);
	Port::WriteU8(CrtcAddr, 15);
	Port::WriteU8(CrtcData, chrOffset);
}

char console_space[sizeof(VGAConsole)];
