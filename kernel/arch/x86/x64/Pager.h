// Pager.h - Working with page tables.

#ifndef __ARCH_X86_X64_PAGER_H__
#define __ARCH_X86_X64_PAGER_H__

#include <Memory.h>

namespace Kernel
{
	namespace Pager
	{
		constexpr unsigned long ValidSizes = Memory::PGS_4K | Memory::PGS_2M | Memory::PGS_1G;
	}
}

#endif
