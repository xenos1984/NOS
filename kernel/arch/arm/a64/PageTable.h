// PageTable.h - ARMv8 page table class.

#ifndef __ARCH_ARM_A64_PAGETABLE_H__
#define __ARCH_ARM_A64_PAGETABLE_H__

#include INC_BITS(Pager.h)
#include INC_BITS(PageTableEntry.h)
#include INC_PLATFORM(Entry.h)
#include <Symbol.h>
#include <Memory.h>
#include <Console.h>

namespace Kernel
{
	namespace Pager
	{
		static const unsigned long PageSizeOffset = 25;
		static const unsigned int InitialLookupLevel = (PageSizeOffset + 5 * GranuleSize - 76) / (GranuleSize - 3);

		static const uintptr_t MaxUserVirt = (1ULL << (64 - PageSizeOffset)) - 1;
		static const uintptr_t MinKernelVirt = ~((1ULL << (64 - PageSizeOffset)) - 1);

		static const unsigned long PageRecursiveKernel = 510;
		static const unsigned long PageRecursiveUser   = 509;
/*
		template<unsigned int> constexpr uintptr_t PageTableAddrKernel = ~0;
		template<unsigned int> constexpr uintptr_t PageTableAddrUser = ~0;

		template<> inline constexpr uintptr_t PageTableAddrKernel<3> = MinKernelVirt + (PageRecursiveKernel << (67 - PageSizeOffset - GranuleSize));
		template<> inline constexpr uintptr_t PageTableAddrUser<3> = MinKernelVirt + (PageRecursiveUser << (67 - PageSizeOffset - GranuleSize));
*/
		constexpr uintptr_t PageTableAddr(unsigned int level, unsigned long rec)
		{
			uintptr_t base = ((level == 3) ? MinKernelVirt : PageTableAddr(level + 1, rec));
			return base + (rec << (64 - PageSizeOffset - (GranuleSize - 3) * (4 - level)));
		}

		/** Page table at a fixed level in the paging hierarchy */
		template<unsigned int level> class alignas(1ULL << GranuleSize) PageTableLevel
		{
		private:
			/** Number of entries. */
			static const unsigned long size = 1ULL << (GranuleSize - 3);

			/** Array of page table entries, default constructed. */
			PageTableEntry entry[size] = {PageTableEntry{}};

		public:
			/** Reference to i'th entry in the table. */
			inline PageTableEntry& Entry(unsigned int i);

			/** Check whether table is completely empty. */
			bool IsEmpty(void);

			/** Return i if this is the i'th table at this level. */
			inline unsigned long Index(void);

			/** Reference to the i'th table at this level. */
			static inline PageTableLevel<level>& Table(bool kernel, unsigned long i);

			/** Page table which contains the page table entry pointing to this page table. */
			inline PageTableLevel<level - 1>& Parent(void);

			/** Page table entry which points to this page table. */
			inline PageTableEntry& Pointer(void);

			/** Check whether the i'th table at this level exists. */
			static bool Exists(bool kernel, unsigned long i);

			/** Create new page table at this level. */
			static PageTableLevel<level>& Create(bool kernel, unsigned long i, Memory::MemType);

			/** Destroy a page table. */
			void Destroy(void);
		} PACKED;

		template<unsigned int level> inline PageTableEntry& PageTableLevel<level>::Entry(unsigned int i)
		{
			return entry[i];
		}

		template<unsigned int level> bool PageTableLevel<level>::IsEmpty(void)
		{
			for(unsigned int i = 0; i < size; i++)
			{
				if(!entry[i].IsClear())
					return false;
			}

			return true;
		}

		template<unsigned int level> inline unsigned long PageTableLevel<level>::Index(void)
		{
			static_assert(level < 4, "Table level exceeds number of paging levels.");
			static_assert(level >= InitialLookupLevel, "Table level below initial lookup level.");

			return (reinterpret_cast<uintptr_t>(this) - (reinterpret_cast<uintptr_t>(this) & -(1ULL << (GranuleSize + (level - InitialLookupLevel) * (GranuleSize - 3))))) >> GranuleSize;
		}

		template<unsigned int level> inline PageTableLevel<level>& PageTableLevel<level>::Table(bool kernel, unsigned long i)
		{
			static_assert(level < 4, "Table level exceeds number of paging levels.");
			static_assert(level >= InitialLookupLevel, "Table level below initial lookup level.");

			return reinterpret_cast<PageTableLevel<level>*>(PageTableAddr(level, (kernel ? PageRecursiveKernel : PageRecursiveUser)))[i];
		}

		template<unsigned int level> inline PageTableLevel<level - 1>& PageTableLevel<level>::Parent(void)
		{
			constexpr uintptr_t mask1 = -(1ULL << (67 - PageSizeOffset - GranuleSize));
			constexpr uintptr_t mask2 = ~mask1 & -(1ULL << GranuleSize);

			uintptr_t tabaddr = reinterpret_cast<uintptr_t>(this);
			uintptr_t shifted = (tabaddr >> (GranuleSize - 3)) & mask2;
			uintptr_t addr = (tabaddr & mask1) | shifted;

			return *reinterpret_cast<PageTableLevel<level - 1>*>(addr);
		}

		template<unsigned int level> inline PageTableEntry& PageTableLevel<level>::Pointer(void)
		{
			return Parent().Entry(Index() & ((1 << (GranuleSize - 3)) - 1));
		}

		template<unsigned int level> bool PageTableLevel<level>::Exists(bool kernel, unsigned long i)
		{
			static_assert(level < 4, "Table level exceeds number of paging levels.");
			static_assert(level >= InitialLookupLevel, "Table level below initial lookup level.");

			if(!PageTableLevel<level - 1>::Exists(kernel, i >> (GranuleSize - 3)))
				return false;

			return !Table(kernel, i).Pointer().IsInvalid();
		}

		template<> inline bool PageTableLevel<InitialLookupLevel>::Exists(bool kernel __attribute__((unused)), unsigned long i __attribute__((unused)))
		{
			return true;
		}

		inline PageTableLevel<InitialLookupLevel>& PageTableTop(bool kernel)
		{
			return PageTableLevel<InitialLookupLevel>::Table(kernel, 0);
		}
	}
}

#endif
