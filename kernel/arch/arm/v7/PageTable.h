// PageTable.h - ARMv7 page table class.

#ifndef __ARCH_ARM_V7_PAGETABLE_H__
#define __ARCH_ARM_V7_PAGETABLE_H__

#include INC_SUBARCH(PageTableEntry.h)
#include <Symbol.h>

namespace Kernel
{
	namespace Pager
	{
		class alignas(16384) PageTableL1
		{
			PageTableEntryL1 entry[4096] = {PageTableEntryL1{}};
		};

		class alignas(1024) PageTableL2
		{
			PageTableEntryL2 entry[256] = {PageTableEntryL2{}};
		};

		inline PageTableL1& KernelPTL1(void)
		{
			return *reinterpret_cast<PageTableL1*>(Symbol::tabPGDIR.Ptr());
		}
	}
}

#endif

