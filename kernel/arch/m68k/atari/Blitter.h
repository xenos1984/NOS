// Blitter.h - Atari blitter.

#ifndef __ARCH_M68K_ATARI_BLITTER_H__
#define __ARCH_M68K_ATARI_BLITTER_H__

#include <cstdint>

namespace Kernel
{
	class Blitter
	{
	private:
		struct _blitter
		{
			volatile uint16_t halftone[16];
			volatile uint16_t sxinc;
			volatile uint16_t syinc;
			volatile uint32_t saddr;
			volatile uint16_t endmask1;
			volatile uint16_t endmask2;
			volatile uint16_t endmask3;
			volatile uint16_t dxinc;
			volatile uint16_t dyinc;
			volatile uint32_t daddr;
			volatile uint16_t xcount;
			volatile uint16_t ycount;
			volatile uint8_t hop;
			volatile uint8_t op;
			volatile uint8_t linenum;
			volatile uint8_t skew;
		} PACKED;

		inline static _blitter& _b(void) { return(*(_blitter*)0xff8a00); };

		static const uint8_t BUSY = 0x80;
		static const uint8_t HOG = 0x40;
		static const uint8_t SMUDGE = 0x20;

		static const uint8_t FXSR = 0x80;
		static const uint8_t NFSR = 0x40;

		inline static void Wait(void)
		{
			while((_b().linenum & BUSY) == BUSY) ;
		}

		inline static void Start(void)
		{
			_b().linenum |= BUSY;
		}

		inline static bool Busy(void)
		{
			return((_b().linenum & BUSY) == BUSY);
		}

	public:
		enum BitOps : uint8_t
		{
			OP_FALSE = 0,
			OP_S_AND_D = 1,
			OP_S_AND_NOT_D = 2,
			OP_S = 3,
			OP_NOT_S_AND_D = 4,
			OP_D = 5,
			OP_S_XOR_D = 6,
			OP_S_OR_D = 7,
			OP_NOT_S_AND_NOT_D = 8,
			OP_S_EQ_D = 9,
			OP_NOT_D = 10,
			OP_S_OR_NOT_D = 11,
			OP_NOT_S = 12,
			OP_NOT_S_OR_D = 13,
			OP_NOT_S_OR_NOT_D = 14,
			OP_TRUE = 15
		};

		enum HalftoneOps : uint8_t
		{
			HOP_TRUE = 0,
			HOP_H = 1,
			HOP_S = 2,
			HOP_S_AND_H = 3
		};

		static void BitBlt(bool wait, void* source, void* dest, int16_t xs, int16_t ys, int16_t xd, int16_t yd, uint16_t xc, uint16_t yc, uint8_t cop = OP_S, uint8_t chop = HOP_S, uint16_t* const ht = nullptr, uint16_t em1 = 0xffff, uint16_t em2 = 0xffff, uint16_t em3 = 0xffff);
	};
}

#endif
