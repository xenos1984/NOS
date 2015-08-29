// Pager.h - Working with page tables.

#ifndef __ARCH_X86_PAGER_H__
#define __ARCH_X86_PAGER_H__

#include <limits>
#include <Memory.h>

namespace Kernel
{
	namespace Pager
	{
		enum PageTableEntryFlags
		{
			PAGE_PRESENT       = 0x0001UL,
			PAGE_WRITEABLE     = 0x0002UL,
			PAGE_USER          = 0x0004UL,
			PAGE_WRITE_THROUGH = 0x0008UL,
			PAGE_CACHE_DISABLE = 0x0010UL,
			PAGE_ACCESSED      = 0x0020UL,
			PAGE_DIRTY         = 0x0040UL,
			PAGE_LARGE         = 0x0080UL,
			PAGE_GLOBAL        = 0x0100UL
		};
	}
}

#include INC_SUBARCH(Pager.h)

#endif
