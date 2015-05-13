// AtariConsole.cpp - Atari console driver for the kernel.

#include <cstddef>
#include INC_SUBARCH(AtariConsole.h)
#include <Fonts.h>
#include INC_SUBARCH(Video.h)
#include INC_SUBARCH(Blitter.h)
#include INC_SUBARCH(MFP.h)
#include INC_SUBARCH(SysVars.h)

using namespace Kernel;

SECTION(".init.text") AtariConsole::AtariConsole(void)
{
	attrib = 0x07;
	if(MFP::IsColorMonitor())
	{
		numLines = 25;
		numColumns = 40;
		numPlanes = 4;
		height = 8;
		font = font8x8t;
	}
	else
	{
		numLines = 25;
		numColumns = 80;
		numPlanes = 1;
		height = 16;
		font = font8x16t;
	}
	xPos = yPos = 0;
	bytesPerLine = height * numPlanes * numColumns;
	totalBytes = numLines * bytesPerLine;

	videoMemory = (volatile unsigned char*)sysvars.VideoBase;
}

AtariConsole::~AtariConsole(void)
{
}

void AtariConsole::Clear(void)
{
	Blitter::BitBlt(true, nullptr, (void*)videoMemory, 2, 2, 2, 2, numColumns * numPlanes / 2, height * numLines, Blitter::OP_FALSE);
	xPos = yPos = 0;
}

uint8_t AtariConsole::GetBackground(void)
{
	return (attrib & 0x70);
}

void AtariConsole::SetBackground(uint8_t value)
{
	attrib &= 0x8f;
	attrib |= value;
}

uint8_t AtariConsole::GetForeground(void)
{
	return (attrib & 0x0f);
}

void AtariConsole::SetForeground(uint8_t value)
{
	attrib &= 0xf0;
	attrib |= value;
}

void AtariConsole::putChar(unsigned char c)
{
	int i, j;

	if(c == '\t')
	{
		xPos += (8 - (xPos & 7));
		if(xPos < numColumns)
			return;
	}
	else if(c != '\n' && c != '\r')
	{
		for(i = 0; i < height; i++)
			for(j = 0; j < numPlanes; j++)
				videoMemory[(xPos & 1) + (xPos & ~1) * numPlanes + (yPos * height + i) * numColumns * numPlanes + 2 * j] = ((0xff * ((attrib >> j) & 1)) & font[256 * i + c]) | ((0xff * ((attrib >> (j + 4)) & 1)) & ~font[256 * i + c]);

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
		Blitter::BitBlt(true, (void*)(videoMemory + bytesPerLine), (void*)videoMemory, 2, 2, 2, 2, numColumns * numPlanes / 2, height * (numLines - 1));
		// Clear the last line.
		Blitter::BitBlt(true, nullptr, (void*)(videoMemory + totalBytes - bytesPerLine), 2, 2, 2, 2, numColumns * numPlanes / 2, height, Blitter::OP_FALSE);
	}
}

void AtariConsole::moveCursor()
{
}

char console_space[sizeof(AtariConsole)];
