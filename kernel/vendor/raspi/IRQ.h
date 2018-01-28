// IRQ.h - Raspberry Pi interrupt controller

#ifndef __VENDOR_RASPI_IRQ_H__
#define __VENDOR_RASPI_IRQ_H__

#include <cstdint>
#include INC_VENDOR(Entry.h)

namespace Kernel
{
	namespace IRQ
	{
		static const uintptr_t base_addr = PERI_OFFSET + 0x0000b000;

		enum Register : int
		{
			REG_PEND_BAS    = 0x200 / 4,
			REG_PEND_1      = 0x204 / 4,
			REG_PEND_2      = 0x208 / 4,
			REG_FIQ_CTRL    = 0x20c / 4,
			REG_ENABLE_1    = 0x210 / 4,
			REG_ENABLE_2    = 0x214 / 4,
			REG_ENABLE_BAS  = 0x218 / 4,
			REG_DISABLE_1   = 0x21c / 4,
			REG_DISABLE_2   = 0x220 / 4,
			REG_DISABLE_BAS = 0x224 / 4
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
