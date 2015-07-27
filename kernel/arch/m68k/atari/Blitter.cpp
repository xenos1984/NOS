// Blitter.cpp - Atari blitter.

#include <cstdint>
#include INC_SUBARCH(Blitter.h)
#include INC_SUBARCH(SysVars.h)
#include <Fonts.h>

using namespace Kernel;

void Blitter::BitBlt(bool wait, void* source, void* dest, uint16_t xs, uint16_t ys, uint16_t xd, uint16_t yd, uint16_t xc, uint16_t yc, uint8_t cop, uint8_t chop, uint16_t em1, uint16_t em2, uint16_t em3)
{
	Wait();
	_b().op = cop;
	_b().hop = chop;
	_b().saddr = (uint32_t)source;
	_b().sxinc = xs;
	_b().syinc = ys;
	_b().daddr = (uint32_t)dest;
	_b().dxinc = xd;
	_b().dyinc = yd;
	_b().xcount = xc;
	_b().ycount = yc;
	_b().endmask1 = em1;
	_b().endmask2 = em2;
	_b().endmask3 = em3;
	Start();
	if(wait)
		Wait();
}
