// Video.cpp - Atari video controller.

#include <cstdint>
#include INC_SUBARCH(Video.h)
#include INC_SUBARCH(SysVars.h)

using namespace Kernel;

static const uint16_t defcolors[16] SECTION(".init.data") =
{
	0x0000,
	0x0700,
	0x0070,
	0x0770,
	0x0007,
	0x0707,
	0x0077,
	0x0555,
	0x0333,
	0x0733,
	0x0373,
	0x0773,
	0x0337,
	0x0737,
	0x0377,
	0x0777
};

SECTION(".init.text") void Video::Init(void)
{
	SetVideoMemory((void*)(sysvars.PhysTop - 32 * 1024));
	SetShiftMode(2);
	SetPalette(defcolors);
}

void Video::SetShiftMode(uint8_t mode)
{
	_v().shiftmode = mode;
	sysvars.CurShifterMode = mode;
}

uint8_t Video::GetShiftMode(void)
{
	return sysvars.CurShifterMode;
}

void Video::SetVideoMemory(void* mem)
{
	_v().vidmemhigh = ((uint32_t)mem) >> 16;
	_v().vidmemmid = ((uint32_t)mem) >> 8;
	sysvars.VideoBase = (uint32_t)mem;
}

void* Video::GetVideoMemory(void)
{
	return (void*)sysvars.VideoBase;
}

void Video::SetPalette(uint8_t index, uint16_t value)
{
	_v().color[index] = value;
}

void Video::SetPalette(const uint16_t* values)
{
	int i;

	for(i = 0; i < 16; i++)
		_v().color[i] = values[i];
}
