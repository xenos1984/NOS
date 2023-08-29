// Pager.h - Working with page tables.

#ifndef __ARCH_X86_PAGER_H__
#define __ARCH_X86_PAGER_H__

#include <Memory.h>

namespace Kernel
{
	namespace Pager
	{
		template<unsigned int level> class PageTableLevel;

		struct Context
		{
			PageTableLevel<0>* top;
			Memory::PhysAddr cr3;
		};

		PageTableLevel<0>* CreateContext(void);
		Memory::PhysAddr SwitchContext(Memory::PhysAddr phys);
		void DeleteContext(PageTableLevel<0>* pt);

		void* MapBootRegion(Memory::PhysAddr start, Memory::PhysAddr length, Memory::MemType type);
		void* MapBootString(Memory::PhysAddr start, Memory::MemType type);
		void FreeBootMemory(void);
	}
}

#include INC_BITS(Pager.h)

#endif
