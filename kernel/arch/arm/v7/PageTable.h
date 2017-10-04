// PageTable.h - ARMv7 page table class.

#ifndef __ARCH_ARM_V7_PAGETABLE_H__
#define __ARCH_ARM_V7_PAGETABLE_H__

#include INC_SUBARCH(PageTableEntry.h)
#include <Symbol.h>

namespace Kernel
{
	namespace Pager
	{
		template<Memory::PageBits size> class alignas(size >> 18) PageTableL1
		{
			PageTableEntryL1 entry[size >> 20] = {PageTableEntryL1{}};
		};

		class alignas(1024) PageTableL2
		{
			PageTableEntryL2 entry[256] = {PageTableEntryL2{}};
		};

		typedef PageTableL1<Memory::PGB_4G> PageTableL1K;

		inline PageTableL1K& KernelPTL1(void)
		{
			return *reinterpret_cast<PageTableL1K*>(Symbol::tabPGDIR.Ptr());
		}
	}
}

#endif

