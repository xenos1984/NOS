// PageTable.h - ARMv7 page table class.

#ifndef __ARCH_ARM_V7_PAGETABLE_H__
#define __ARCH_ARM_V7_PAGETABLE_H__

#include INC_SUBARCH(PageTableEntry.h)
#include INC_VENDOR(Entry.h)
#include <Symbol.h>
#include <Memory.h>

namespace Kernel
{
	namespace Pager
	{
		static const unsigned long MinKernelL1Entry = 4096 >> TABLE_SPLIT_BITS;
		static const unsigned long MaxUserL1Entry = MinKernelL1Entry - 1;
		static const uintptr_t MinKernelVirt = MinKernelL1Entry << Memory::PGB_1M;
		static const uintptr_t MaxUserVirt = MinKernelVirt - 1;
		static const uintptr_t KernelPTL2Base = KERNEL_OFFSET;
		static const uintptr_t UserPTL2Base = MinKernelVirt - (Memory::PGS_4M >> TABLE_SPLIT_BITS);
		static const uintptr_t UserPTL1Base = UserPTL2Base - (Memory::PGS_16K >> TABLE_SPLIT_BITS);
		static const Memory::PageBits UserPageBits = (Memory::PageBits)(Memory::PGB_4G - TABLE_SPLIT_BITS);

		class PageTableTop
		{
		public:
			static inline PageTableEntryL1& Entry(unsigned int i);
		};

		template<Memory::PageBits size> class alignas(1 << (size - 18)) PageTableL1 : public PageTableTop
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

		typedef PageTableL1<UserPageBits> PageTableL1U;

		inline PageTableL1U& UserPTL1(void)
		{
			return *reinterpret_cast<PageTableL1U*>(UserPTL1Base);
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
			if(i < MinKernelL1Entry)
				return *reinterpret_cast<PageTableL2*>(UserPTL2Base + i * sizeof(PageTableL2));
			else
				return *reinterpret_cast<PageTableL2*>(KernelPTL2Base + i * sizeof(PageTableL2));
		}

		inline bool PageTableL2::Exists(unsigned long i)
		{
			return KernelPTL1().Entry(i).IsTable();
		}

		inline PageTableEntryL1& PageTableTop::Entry(unsigned int i)
		{
			if(i < MinKernelL1Entry)
				return UserPTL1().Entry(i);
			else
				return KernelPTL1().Entry(i);
		}
	}
}

#endif

