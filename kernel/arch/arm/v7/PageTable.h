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
		private:
			PageTableEntryL1 entry[size >> 20] = {PageTableEntryL1{}};

		public:
			inline PageTableEntryL1& Entry(unsigned int i)
			{
				return entry[i];
			}
		};

		class alignas(1024) PageTableL2
		{
		private:
			PageTableEntryL2 entry[256] = {PageTableEntryL2{}};

		public:
			inline PageTableEntryL2& Entry(unsigned int i)
			{
				return entry[i];
			}

			static inline PageTableL2& Table(unsigned long i)
			{
				// TODO: Use some meaningful constants here
				if(i < 2048)
					return *reinterpret_cast<PageTableL2*>(0x7fe00000 + i * sizeof(PageTableL2));
				else
					return *reinterpret_cast<PageTableL2*>(0xc0000000 + i * sizeof(PageTableL2));
			}
		};

		typedef PageTableL1<Memory::PGB_4G> PageTableL1K;

		inline PageTableL1K& KernelPTL1(void)
		{
			return *reinterpret_cast<PageTableL1K*>(Symbol::tabPGDIR.Ptr());
		}
	}
}

#endif

