// Memory.h - General memory information.

#ifndef __ARCH_X86_MEMORY_H__
#define __ARCH_X86_MEMORY_H__

namespace Kernel
{
	namespace Memory
	{
		static const PageBits MinPageBits = PAGE_4K; /**< Smallest page has 4kB. */
		static const unsigned long MaxInitPages = 15UL << (20 - MinPageBits); /**< Initially at most 15MB (from 1MB to 16MB) are used. */

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

#include INC_SUBARCH(Memory.h)

#endif
