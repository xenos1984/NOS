// Pager.h - Working with page tables.

#ifndef __ARCH_X86_PAGER_H__
#define __ARCH_X86_PAGER_H__

#include <Memory.h>

namespace Kernel
{
	namespace Pager
	{
		void* MapBootRegion(Memory::PhysAddr start, Memory::PhysAddr length, Memory::MemType type);
		void* MapBootString(Memory::PhysAddr start, Memory::MemType type);
		void FreeBootMemory(void);
	}
}

#include INC_SUBARCH(Pager.h)

#endif
