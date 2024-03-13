// Timer.h - Raspberry Pi timer

#ifndef __PLATFORM_RASPI_TIMER_H__
#define __PLATFORM_RASPI_TIMER_H__

#include <cstdint>
#include INC_PLATFORM(Entry.h)

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

		inline uint32_t CounterLow(void)
		{
			return Read(REG_CLO);
		}

		inline uint32_t CounterHigh(void)
		{
			return Read(REG_CHI);
		}

		inline uint64_t Counter(void)
		{
			return CounterLow() + (((uint64_t)CounterHigh()) << 32);
		}

		inline bool IsMatch(int n)
		{
			return Read(REG_CS) & (1 << n);
		}

		inline void ClearMatch(int n)
		{
			Write(REG_CS, 1 << n);
		}

		inline uint32_t GetCompare(int n)
		{
			return Read(REG_C0 + n);
		}

		inline void SetCompare(int n, uint32_t value)
		{
			Write(REG_C0 + n, value);
		}
	}
}

#endif
