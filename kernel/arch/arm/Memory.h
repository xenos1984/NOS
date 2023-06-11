// Memory.h - General memory information.

#ifndef __ARCH_ARM_MEMORY_H__
#define __ARCH_ARM_MEMORY_H__

#include <cstdint>

namespace Kernel
{
	namespace Memory
	{
		static const unsigned long MaxInitPages = 1UL << (PGB_1G - PGB_4K); /**< Initially at most 1GB are used. */

		enum class Zone
		{
			NONE,
			MAX = NONE
		};
#ifdef ARCH_ARM_V8
		typedef uint64_t PhysAddr;
#else
		typedef uint32_t PhysAddr;
#endif
	}
}

#endif
