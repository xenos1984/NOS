// Pager.h - Working with page tables.

#ifndef __ARCH_X86_IA32_PAGER_H__
#define __ARCH_X86_IA32_PAGER_H__

#include <cstdint>

namespace Kernel
{
	namespace Pager
	{
		enum PageBits : int
		{
			PAGE_4K = 12,
			PAGE_4M = 22
		};

		static const int AllowedSizes = (1UL << PAGE_4K) | (1UL << PAGE_4M);

		constexpr bool IsAllowedSize(PageBits size)
		{
//			return true;
			return(AllowedSizes & (1UL << size) != 0);
		}
	}
}

#endif
