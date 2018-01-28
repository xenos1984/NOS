// Timer.h - Raspberry Pi timer

#ifndef __VENDOR_RASPI_TIMER_H__
#define __VENDOR_RASPI_TIMER_H__

#include <cstdint>
#include INC_VENDOR(Entry.h)

namespace Kernel
{
	namespace Timer
	{
		static const uintptr_t base_addr = PERI_OFFSET + 0x00003000;

		enum Register : int
		{
			REG_CS  = 0x00 / 4,
			REG_CLO = 0x04 / 4,
			REG_CHI = 0x08 / 4,
			REG_C0  = 0x0c / 4,
			REG_C1  = 0x10 / 4,
			REG_C2  = 0x14 / 4,
			REG_C3  = 0x18 / 4
		};

		inline void Write(int reg, uint32_t value)
		{
			((volatile uint32_t*)base_addr)[reg] = value;
		}

		inline uint32_t Read(int reg)
		{
			return ((volatile uint32_t*)base_addr)[reg];
		}
	}
}

#endif
