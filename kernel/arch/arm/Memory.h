// Memory.h - General memory information.

#ifndef __ARCH_ARM_MEMORY_H__
#define __ARCH_ARM_MEMORY_H__

#include <cstdint>

namespace Kernel
{
	namespace Memory
	{
		static const PageBits MinPageBits = PGB_4K; /**< Smallest page has 4kB. */
		static const unsigned long MaxInitPages = 1UL << (PGB_1G - MinPageBits); /**< Initially at most 1GB are used. */

		enum class Zone
		{
			NONE,
			MAX = NONE
		};

		typedef uint32_t PhysAddr;
	}
}

#endif
