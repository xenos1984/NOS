// PageTable.h - ARMv8 page table class.

#ifndef __ARCH_ARM_V8_PAGETABLE_H__
#define __ARCH_ARM_V8_PAGETABLE_H__

#include INC_SUBARCH(PageTableEntry.h)
#include INC_VENDOR(Entry.h)
#include <Symbol.h>
#include <Memory.h>

namespace Kernel
{
	namespace Pager
	{
		static const unsigned long PageSizeOffset = 25;
		static const Memory::PageBits GranuleSize = Memory::PGB_4K;
		static const unsigned int InitialLookupLevel = (PageSizeOffset + 5 * GranuleSize - 76) / (GranuleSize - 3);

		static const uintptr_t MaxUserVirt = (1ULL << (64 - PageSizeOffset)) - 1;
		static const uintptr_t MinKernelVirt = ~((1ULL << (64 - PageSizeOffset)) - 1);

		static const unsigned long PageRecursiveKernel = 510;
		static const unsigned long PageRecursiveUser = PageRecursiveKernel - 1;
/*
		template<unsigned int> constexpr uintptr_t PageTableAddrKernel = ~0;
		template<unsigned int> constexpr uintptr_t PageTableAddrUser = ~0;

		template<> constexpr uintptr_t PageTableAddrKernel<3> = MinKernelVirt + (PageRecursiveKernel << (67 - PageSizeOffset - GranuleSize));
		template<> constexpr uintptr_t PageTableAddrUser<3> = MinKernelVirt + (PageRecursiveUser << (67 - PageSizeOffset - GranuleSize));
*/
		constexpr uintptr_t PageTableAddr(unsigned int level, unsigned long rec)
		{
			uintptr_t base = ((level == 3) ? MinKernelVirt : PageTableAddr(level + 1, rec));
			return base + (rec << (64 - PageSizeOffset - (GranuleSize - 3) * (4 - level)));
		}

		/** Page table at a fixed level in the paging hierarchy */
		template<unsigned int level> class alignas(1ULL << GranuleSize) PageTableLevel
		{
		protected:
			/** Array of page table entries, default constructed. */
			PageTableEntry entry[1ULL << (GranuleSize - 3)] = {PageTableEntry{}};

		public:
			/** Reference to i'th entry in the table. */
			inline PageTableEntry& Entry(unsigned int i);

			/** Check whether table is completely empty. */
			bool IsEmpty(void);

			/** Return i if this is the i'th table at this level. */
			inline unsigned long Index(void);

			/** Reference to the i'th table at this level. */
			static inline PageTableLevel<level>& Table(unsigned long i);

			/** Page table which contains the page table entry pointing to this page table. */
			inline PageTableLevel<level - 1>& Parent(void);

			/** Page table entry which points to this page table. */
			inline PageTableEntry& Pointer(void);

			/** Check whether the i'th table at this level exists. */
			static bool Exists(unsigned long i);

			/** Create new page table at this level. */
			static PageTableLevel<level>& Create(unsigned long i, Memory::MemType);

			/** Destroy a page table. */
			void Destroy(void);
		} PACKED;
	}
}

#endif
