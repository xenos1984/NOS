// PageTable.h - ARMv7 page table class.

#ifndef __ARCH_ARM_V7_PAGETABLE_H__
#define __ARCH_ARM_V7_PAGETABLE_H__

#include INC_SUBARCH(PageTableEntry.h)
#include <Symbol.h>
#include <Memory.h>

namespace Kernel
{
	namespace Pager
	{
		template<Memory::PageBits size> class alignas(1 << (size - 18)) PageTableL1
		{
		private:
			/** Array of page table entries, default constructed. */
			PageTableEntryL1 entry[1 << (size - 20)] = {PageTableEntryL1{}};

		public:
			/** Reference to i'th entry in the table. */
			inline PageTableEntryL1& Entry(unsigned int i);
		};

		class alignas(1024) PageTableL2
		{
		private:
			/** Array of page table entries, default constructed. */
			PageTableEntryL2 entry[256] = {PageTableEntryL2{}};

		public:
			/** Reference to i'th entry in the table. */
			inline PageTableEntryL2& Entry(unsigned int i);

			/** Reference to the i'th L2 page table. */
			static inline PageTableL2& Table(unsigned long i);

			/** Check whether the i'th L2 page table exists. */
			static inline bool Exists(unsigned long i);

			/** Create new L2 page table. */
			static PageTableL2& Create(unsigned long i, Memory::MemType);

			/** Destroy a L2 page table. */
			void Destroy(void);

			/** Check whether table is completely empty. */
			bool IsEmpty(void);
		};

		typedef PageTableL1<Memory::PGB_4G> PageTableL1K;

		inline PageTableL1K& KernelPTL1(void)
		{
			return *reinterpret_cast<PageTableL1K*>(Symbol::tabPGDIR.Ptr());
		}

		template<Memory::PageBits size> inline PageTableEntryL1& PageTableL1<size>::Entry(unsigned int i)
		{
			return entry[i];
		}

		inline PageTableEntryL2& PageTableL2::Entry(unsigned int i)
		{
			return entry[i];
		}

		inline PageTableL2& PageTableL2::Table(unsigned long i)
		{
			// TODO: Use some meaningful constants here
			if(i < 2048)
				return *reinterpret_cast<PageTableL2*>(0x7fe00000 + i * sizeof(PageTableL2));
			else
				return *reinterpret_cast<PageTableL2*>(0xc0000000 + i * sizeof(PageTableL2));
		}

		inline bool PageTableL2::Exists(unsigned long i)
		{
			return KernelPTL1().Entry(i).IsTable();
		}
	}
}

#endif

