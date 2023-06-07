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
		static const unsigned long PageTablesTotal = 0x1000;
		static const unsigned long PageTablesUser = PageTablesTotal >> TABLE_SPLIT_BITS;
		static const unsigned long PageTablesKernel = PageTablesTotal - PageTablesUser;
		static const uintptr_t KernelVirtStart = PageTablesUser << Memory::PGB_1M;
		static const uintptr_t KernelPTL2Base = KERNEL_OFFSET + (Memory::PGS_4M >> TABLE_SPLIT_BITS);
		static const uintptr_t UserPTL2Base = KernelVirtStart - (Memory::PGS_4M >> TABLE_SPLIT_BITS);
		static const uintptr_t UserPTL1Base = UserPTL2Base - (Memory::PGS_16K >> TABLE_SPLIT_BITS);
		static const Memory::PageBits UserPageBits = (Memory::PageBits)(Memory::PGB_4G - TABLE_SPLIT_BITS);

		/** Base class for top level page table. */
		class PageTableTop
		{
		public:
			static inline PageTableEntryL1& Entry(unsigned int i);
		};

		/** First level page table responsble for given amount of memory - see specializations below for values. */
		template<Memory::PageBits size> class alignas(1 << (size - 18)) PageTableL1 : public PageTableTop
		{
		private:
			/** Array of page table entries, default constructed. */
			PageTableEntryL1 entry[1 << (size - 20)] = {PageTableEntryL1{}};

		public:
			/** Reference to i'th entry in the table. */
			inline PageTableEntryL1& Entry(unsigned int i);
		};

		/** Reference to i'th entry. */
		template<Memory::PageBits size> inline PageTableEntryL1& PageTableL1<size>::Entry(unsigned int i)
		{
			return entry[i];
		}

		/** Kernel top level page table: full size, address up to 4G. */
		typedef PageTableL1<Memory::PGB_4G> PageTableL1K;

		/** User top level page table: reduced size, address user space only. */
		typedef PageTableL1<UserPageBits> PageTableL1U;

		/** Kernel top level page table defined in linker script. */
		extern "C" Symbol::ExtObject<PageTableL1K> tabPGDIR;

		/** User top level page table mapped into each address space. */
		static Symbol::ConstObject<PageTableL1U, UserPTL1Base> UserPTL1;

		/** Reference to top level page table entry, selecting a table depending on index. */
		inline PageTableEntryL1& PageTableTop::Entry(unsigned int i)
		{
			if(i < PageTablesUser)
				return UserPTL1->Entry(i);
			else
				return tabPGDIR->Entry(i);
		}

		/** Second level page table. */
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

		static Symbol::ConstArray<PageTableL2, PageTablesUser, UserPTL2Base> UserPTL2;
		static Symbol::ConstArray<PageTableL2, PageTablesKernel, KernelPTL2Base> KernelPTL2;

		/** Reference to i'th entry. */
		inline PageTableEntryL2& PageTableL2::Entry(unsigned int i)
		{
			return entry[i];
		}

		inline PageTableL2& PageTableL2::Table(unsigned long i)
		{
			if(i < PageTablesUser)
				return UserPTL2[i];
			else
				return KernelPTL2[i - PageTablesUser];
		}

		inline bool PageTableL2::Exists(unsigned long i)
		{
			return tabPGDIR->Entry(i).IsTable();
		}
	}
}

#endif
