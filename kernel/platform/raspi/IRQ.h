// IRQ.h - Raspberry Pi interrupt controller

#ifndef __PLATFORM_RASPI_IRQ_H__
#define __PLATFORM_RASPI_IRQ_H__

#include <cstdint>
#include INC_PLATFORM(Entry.h)

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

		enum BaseIRQ : uint32_t
		{
			BIRQ_TIMER     = 0x01,
			BIRQ_MAILBOX   = 0x02,
			BIRQ_DOORBELL0 = 0x04,
			BIRQ_DOORBELL1 = 0x08,
			BIRQ_GPU_HALT0 = 0x10,
			BIRQ_GPU_HALT1 = 0x20,
			BIRQ_ACC_ERR0  = 0x40,
			BIRQ_ACC_ERR1  = 0x80
		};

		inline void Write(int reg, uint32_t value)
		{
			((volatile uint32_t*)base_addr)[reg] = value;
		}

		inline uint32_t Read(int reg)
		{
			return ((volatile uint32_t*)base_addr)[reg];
		}

		inline void BaseEnable(uint32_t irq)
		{
			Write(REG_ENABLE_BAS, irq);
		}

		inline void BaseDisable(uint32_t irq)
		{
			Write(REG_DISABLE_BAS, irq);
		}
	}
}

#endif
