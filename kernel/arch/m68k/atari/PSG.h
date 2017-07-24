// PSG.h - Programmable sound generator.

#ifndef __ARCH_M68K_ATARI_PSG_H__
#define __ARCH_M68K_ATARI_PSG_H__

#include <cstdint>

namespace Kernel
{
	class PSG
	{
	private:
		struct _psg
		{
			volatile uint8_t addr;
			volatile uint8_t space1;
			volatile uint8_t data;
			volatile uint8_t space2;
		} PACKED;

		inline static _psg& _p(void) { return(*(_psg*)0xff8800); };

		enum RegIndex : uint8_t
		{
			PER_A_LOW     = 0x0,
			PER_A_HIGH    = 0x1,
			PER_B_LOW     = 0x2,
			PER_B_HIGH    = 0x3,
			PER_C_LOW     = 0x4,
			PER_C_HIGH    = 0x5,
			NOISE         = 0x6,
			MIXER         = 0x7,
			AMP_A         = 0x8,
			AMP_B         = 0x9,
			AMP_C         = 0xa,
			PER_HULL_LOW  = 0xb,
			PER_HULL_HIGH = 0xc,
			SHAPE_HULL    = 0xd,
			IO_A          = 0xe,
			IO_B          = 0xf
		};

	public:
		inline static void Test(void)
		{
			_p().addr = PER_A_LOW;
			_p().data = 0;
			_p().addr = PER_A_HIGH;
			_p().data = 2;
			_p().addr = AMP_A;
			_p().data = 10;
			_p().addr = MIXER;
			_p().data = 0xfe;
		}
	};
}

#endif
