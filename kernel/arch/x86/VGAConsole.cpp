// VGAConsole.cpp - VGA console driver for the kernel.

#include INC_ARCH(VGAConsole.h)
#include <Symbol.h>
#include INC_ARCH(Port.h)

using namespace Kernel;

SECTION(".init.text") VGAConsole::VGAConsole(void)
{
	attrib = 0x07;
	numLines = LINES;
	numColumns = COLUMNS;
	xPos = yPos = 0;
}

VGAConsole::~VGAConsole(void)
{
}

void VGAConsole::Clear(void)
{
	int length = TOTAL_BYTES / sizeof(long);
	unsigned long pattern = clearpat();

	while(length--)
		((unsigned long*)(videoMemory()))[length] = pattern;
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

	Port::WriteU8(DEBUG_PORT, c); // Bochs debugger console

	if(c == '\t')
	{
		xPos += (8 - (xPos & 7));
		if(xPos < COLUMNS)
			return;
	}
	else if(c != '\n' && c != '\r')
	{
		videoMemory()[xPos + yPos * COLUMNS].ch_at.character = c;
		videoMemory()[xPos + yPos * COLUMNS].ch_at.attribute = attrib;

		xPos++;
		if(xPos < COLUMNS)
			return;
	}

	xPos = 0;
	yPos++;
	if(yPos >= LINES)
	{
		yPos = LINES - 1;
		// Scroll up a line.
		t = (char*)videoMemory();
		f = t + BYTES_PER_LINE;
		for(i = 0; i < TOTAL_BYTES - BYTES_PER_LINE; i++)
			t[i] = f[i];
		// Clear the last line.
		for(; i < TOTAL_BYTES; i++)
			t[i] = 0;
	}
}

void VGAConsole::moveCursor(void)
{
	uint16_t chrOffset = xPos + yPos * COLUMNS;

	Port::WriteU8(CRTC_ADDR, 14);
	Port::WriteU8(CRTC_DATA, chrOffset >> 8);
	Port::WriteU8(CRTC_ADDR, 15);
	Port::WriteU8(CRTC_DATA, chrOffset);
}

char console_space[sizeof(VGAConsole)];
