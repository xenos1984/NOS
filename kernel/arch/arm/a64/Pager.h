// Pager.h - Working with page tables.

#ifndef __ARCH_ARM_A8_PAGER_H__
#define __ARCH_ARM_A8_PAGER_H__

#include <Memory.h>

namespace Kernel
{
	namespace Pager
	{
		static const Memory::PageBits GranuleSize = Memory::PGB_4K;
		constexpr unsigned long ValidSizes = (1UL << GranuleSize) | (1UL << (2 * GranuleSize - 3)) | (1UL << (3 * GranuleSize - 6));

		struct Context
		{
		};
	}
}

#endif
