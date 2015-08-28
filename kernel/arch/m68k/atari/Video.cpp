// Video.cpp - Atari video controller.

#include <cstdint>
#include INC_SUBARCH(Video.h)
#include INC_SUBARCH(SysVars.h)

using namespace Kernel;

static const uint16_t defcolors[16] SECTION(".init.data") =
{
	0x0000,
	0x0005,
	0x0050,
	0x0025,
	0x0500,
	0x0505,
	0x0550,
	0x0555,
	0x0222,
	0x0227,
	0x0272,
	0x0277,
	0x0722,
	0x0727,
	0x0772,
	0x0777
};

SECTION(".init.text") void Video::Init(void)
{
	SetVideoMemory((void*)(sysvars.PhysTop - 32 * 1024));
	SetShiftMode(0);
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

uint32_t Video::GetCount(void)
{
	return ((uint32_t)(_v().vidcounthigh) << 16) | ((uint32_t)(_v().vidcountmid) << 8) | (uint32_t)(_v().vidcountlow);
}
