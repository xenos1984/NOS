// Pager.h - Working with page tables.

#ifndef __ARCH_ARM_A7_PAGER_H__
#define __ARCH_ARM_A7_PAGER_H__

#include <Memory.h>

namespace Kernel
{
	namespace Pager
	{
		constexpr unsigned long ValidSizes = Memory::PGS_4K | Memory::PGS_64K | Memory::PGS_1M | Memory::PGS_16M;

		struct Context
		{
		};
	}
}

#endif
