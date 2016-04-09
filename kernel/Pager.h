// Pager.h - Working with page tables.

#ifndef __PAGER_H__
#define __PAGER_H__

#include <Memory.h>

namespace Kernel
{
	namespace Pager
	{
		template<Memory::PageBits bits> bool MapPage(Memory::PhysAddr phys, uintptr_t virt, unsigned int flags);
	}
}

#include INC_ARCH(Pager.h)

#endif
