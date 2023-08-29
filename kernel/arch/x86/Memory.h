// Memory.h - General memory information.

#ifndef __ARCH_X86_MEMORY_H__
#define __ARCH_X86_MEMORY_H__

namespace Kernel
{
	namespace Memory
	{
		static const unsigned long MaxInitPages = 15UL << (PGB_1M - PGB_4K); /**< Initially at most 15MB (from 1MB to 16MB) are used. */

		enum class Zone
		{
			HIGH,
			DMA32,
			DMA24,
			REAL,
			MAX = REAL
		};
	}
}

#include INC_BITS(Memory.h)

#endif
