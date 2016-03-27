// Memory.h - General memory information.

#ifndef __MEMORY_H__
#define __MEMORY_H__

namespace Kernel
{
	namespace Memory
	{
		enum PageBits : unsigned long
		{
			PAGE_1K = 10,
			PAGE_2K,
			PAGE_4K,
			PAGE_8K,
			PAGE_16K,
			PAGE_32K,
			PAGE_64K,
			PAGE_128K,
			PAGE_256K,
			PAGE_512K,
			PAGE_1M,
			PAGE_2M,
			PAGE_4M,
			PAGE_8M,
			PAGE_16M,
			PAGE_32M,
			PAGE_64M,
			PAGE_128M,
			PAGE_256M,
			PAGE_512M,
			PAGE_1G
		};

		constexpr bool IsValidSize(PageBits size, unsigned long mask)
		{
			return((mask & (1UL << size)) != 0);
		}
	}
}

#include INC_ARCH(Memory.h)

#endif
