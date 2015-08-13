// VGAConsole.cpp - VGA console driver for the kernel.

#include INC_ARCH(VGAConsole.h)
#include <Symbol.h>
#include INC_ARCH(Port.h)

using namespace Kernel;

SECTION(".init.text") VGAConsole::VGAConsole(void) : attrib(0x07), xPos(0), yPos(0), bufoffset(0)
{
	numLines = LINES;
	numColumns = COLUMNS;
}

VGAConsole::~VGAConsole(void)
{
}

void VGAConsole::Clear(void)
{
	unsigned int length = TOTAL_BYTES / sizeof(long);
	unsigned long pattern = clearpat();

	asm volatile ("xchgw %bx,%bx");
	while(length--)
	{
		videoMemoryLong()[length] = pattern;
		buffer.raw[length] = pattern;
	}
	xPos = yPos = 0;
	bufoffset = 0;
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
	volatile unsigned long *t;
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
		videoMemoryCh()[xPos + yPos * COLUMNS].ch_at.character = c;
		videoMemoryCh()[xPos + yPos * COLUMNS].ch_at.attribute = attrib;
		buffer.ch_at[(bufoffset + xPos + yPos * COLUMNS) % TOTAL_CHARS].ch_at.character = c;
		buffer.ch_at[(bufoffset + xPos + yPos * COLUMNS) % TOTAL_CHARS].ch_at.attribute = attrib;

		xPos++;
		if(xPos < COLUMNS)
			return;
	}

	xPos = 0;
	yPos++;
	if(yPos >= LINES)
	{
		// Scroll up a line.
		bufoffset += COLUMNS;
		unsigned int ofs = 2 * bufoffset / sizeof(long);
		unsigned int total = TOTAL_BYTES / sizeof(long);
		unsigned long pattern = clearpat();

		yPos = LINES - 1;
		t = videoMemoryLong();
		for(i = 0; i < (TOTAL_BYTES - BYTES_PER_LINE) / sizeof(long); i++)
			t[i] = buffer.raw[(i + ofs) % total];
		// Clear the last line.
		for(; i < TOTAL_BYTES / sizeof(long); i++)
		{
			t[i] = buffer.raw[(i + ofs) % total] = pattern;
		}
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
