// Pager.h - Working with page tables.

#ifndef __PAGER_H__
#define __PAGER_H__

#include <cstddef>
#include <Memory.h>

namespace Kernel
{
	namespace Pager
	{
		template<Memory::PageBits bits> bool MapPage(Memory::PhysAddr phys, uintptr_t virt, unsigned int flags);
		Memory::PageBits MappedSize(uintptr_t virt);
		bool IsMapped(uintptr_t virt, size_t length);
		bool IsUnmapped(uintptr_t virt, size_t length);
		bool Map(Memory::PhysAddr phys, uintptr_t virt, size_t length, unsigned long type);
	}
}

#include INC_ARCH(Pager.h)

#endif
