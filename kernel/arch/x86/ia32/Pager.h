// Pager.h - Working with page tables.

#ifndef __ARCH_X86_IA32_PAGER_H__
#define __ARCH_X86_IA32_PAGER_H__

#include <Memory.h>

namespace Kernel
{
	namespace Pager
	{
		static const unsigned long ValidSizes = Memory::PGS_4K | Memory::PGS_4M;

		constexpr bool IsValidSize(Memory::PageBits size)
		{
			return Memory::IsValidSize(size, ValidSizes);
		}
	}
}

#endif
