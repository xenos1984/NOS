// Memory.h - General memory information.

#ifndef __ARCH_M68K_MEMORY_H__
#define __ARCH_M68K_MEMORY_H__

#include <cstdint>

namespace Kernel
{
	namespace Memory
	{
		typedef uint32_t PhysAddr;
		static const unsigned int MinPageBits = 12;
		static const unsigned long MaxInitPages = 1UL << (20 - MinPageBits);

		enum class Zone
		{
			MAX = 0
		};
	}
}

#endif
