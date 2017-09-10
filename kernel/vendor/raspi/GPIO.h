// GPIO.h - Raspberry Pi general purpose I/O

#ifndef __VENDOR_RASPI_GPIO_H__
#define __VENDOR_RASPI_GPIO_H__

#include <cstdint>
#include INC_VENDOR(Entry.h)

namespace Kernel
{
	namespace GPIO
	{
		static const uintptr_t base_addr = PERI_OFFSET + 0x00200000;

		enum Register : int
		{
			REG_FSEL0   = 0x00 / 4,
			REG_FSEL1   = 0x04 / 4,
			REG_FSEL2   = 0x08 / 4,
			REG_FSEL3   = 0x0c / 4,
			REG_FSEL4   = 0x10 / 4,
			REG_FSEL5   = 0x14 / 4,
			REG_SET0    = 0x1c / 4,
			REG_SET1    = 0x20 / 4,
			REG_CLR0    = 0x28 / 4,
			REG_CLR1    = 0x2c / 4,
			REG_LEV0    = 0x34 / 4,
			REG_LEV1    = 0x38 / 4,
			REG_EDS0    = 0x40 / 4,
			REG_EDS1    = 0x44 / 4,
			REG_REN0    = 0x4c / 4,
			REG_REN1    = 0x50 / 4,
			REG_FEN0    = 0x58 / 4,
			REG_FEN1    = 0x5c / 4,
			REG_HEN0    = 0x64 / 4,
			REG_HEN1    = 0x68 / 4,
			REG_LEN0    = 0x70 / 4,
			REG_LEN1    = 0x74 / 4,
			REG_AREN0   = 0x7c / 4,
			REG_AREN1   = 0x80 / 4,
			REG_AFEN0   = 0x88 / 4,
			REG_AFEN1   = 0x8c / 4,
			REG_PUD     = 0x94 / 4,
			REG_PUDCLK0 = 0x98 / 4,
			REG_PUDCLK1 = 0x9c / 4
		};

		enum Pull : uint32_t
		{
			PULL_NONE = 0x00,
			PULL_DOWN = 0x01,
			PULL_UP   = 0x10
		};

		inline void Write(int reg, uint32_t value)
		{
			((volatile uint32_t*)base_addr)[reg] = value;
		}

		inline uint32_t Read(int reg)
		{
			return ((volatile uint32_t*)base_addr)[reg];
		}

		void TogglePull(unsigned int bit, unsigned int pull);
	}
}

#endif
