// PageTable.h - x86 page table class.

#ifndef __ARCH_X86_PAGETABLE_H__
#define __ARCH_X86_PAGETABLE_H__

#include <type_traits>
#include <new>
#include <Memory.h>
#include <Pager.h>
#include <Chunker.h>
#include INC_ARCH(PageTableEntry.h)

namespace Kernel
{
	namespace Pager
	{
#ifdef ARCH_X86_IA32
#ifdef CONFIG_PAE
		static constexpr unsigned int PAGE_LEVELS = 3;
		static constexpr unsigned int PAGE_BITS[] = {2, 9, 9};
		static constexpr uintptr_t PAGE_TABLE_ADDR[] = {0xfffffff8, 0xffffffe0, 0xffffc000, 0xff800000};
#else
		static constexpr unsigned int PAGE_LEVELS = 2;
		static constexpr unsigned int PAGE_BITS[] = {10, 10};
		static constexpr uintptr_t PAGE_TABLE_ADDR[] = {0xfffffffc, 0xfffff000, 0xffc00000};
#endif
#endif
#ifdef ARCH_X86_AMD64
		static constexpr unsigned int PAGE_LEVELS = 4;
		static constexpr unsigned int PAGE_BITS[] = {9, 9, 9, 9};
		static constexpr uintptr_t PAGE_TABLE_ADDR[] = {0xffffff7fbfdfeff0, 0xffffff7fbfdfe000, 0xffffff7fbfc00000, 0xffffff7f80000000, 0xffffff0000000000};
#endif

		/** Page table with a fixed number of entries. */
		template<unsigned int size> class alignas(size * sizeof(PageTableEntry)) PageTableSize
		{
		protected:
			/** Array of page table entries, default constructed. */
			PageTableEntry entry[size] = {PageTableEntry{}};

		public:
			/** Reference to i'th entry in the table. */
			inline PageTableEntry& Entry(unsigned int i);

			/** Check whether table is completely empty. */
			bool IsEmpty(void);
		} PACKED;

		/** Page table at a fixed level in the paging hierarchy */
		template<unsigned int level> class PageTableLevel : public PageTableSize<1 << PAGE_BITS[level]>
		{
		public:
			PageTableLevel(void) {};

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
		};

		template<unsigned int size> inline PageTableEntry& PageTableSize<size>::Entry(unsigned int i)
		{
			return entry[i];
		}

		template<unsigned int size> bool PageTableSize<size>::IsEmpty(void)
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
			static_assert(level < PAGE_LEVELS, "Table level exceeds number of paging levels.");
			return (reinterpret_cast<uintptr_t>(this) - PAGE_TABLE_ADDR[level + 1]) / sizeof(PageTableLevel<level>);
		}

		template<unsigned int level> inline PageTableLevel<level>& PageTableLevel<level>::Table(unsigned long i)
		{
			static_assert(level < PAGE_LEVELS, "Table level exceeds number of paging levels.");
			return reinterpret_cast<PageTableLevel<level>*>(PAGE_TABLE_ADDR[level + 1])[i];
		}

		template<unsigned int level> inline PageTableLevel<level - 1>& PageTableLevel<level>::Parent(void)
		{
			return PageTableLevel<level - 1>::Table(Index() >> PAGE_BITS[level - 1]);
		}

		template<unsigned int level> inline PageTableEntry& PageTableLevel<level>::Pointer(void)
		{
			return Parent().Entry(Index() & ((1 << PAGE_BITS[level - 1]) - 1));
		}

		template<unsigned int level> bool PageTableLevel<level>::Exists(unsigned long i)
		{
			static_assert(level < PAGE_LEVELS, "Table level exceeds number of paging levels.");

			if(!PageTableLevel<level - 1>::Exists(i >> PAGE_BITS[level]))
				return false;

			return Table(i).Pointer().IsPresent();
		}

		template<> inline bool PageTableLevel<0>::Exists(unsigned long i __attribute__((unused)))
		{
			return true;
		}

		template<unsigned int level> PageTableLevel<level>& PageTableLevel<level>::Create(unsigned long i, Memory::MemType type)
		{
			static_assert(level > 0, "Top level page table cannot be created.");
			static_assert(level < PAGE_LEVELS, "Table level exceeds number of paging levels.");

			uintptr_t virt = PAGE_TABLE_ADDR[level + 1] + i * sizeof(PageTableLevel<level>);
			Memory::AllocBlock<Memory::PGB_4K>(virt, type);
			new (reinterpret_cast<PageTableLevel<level>*>(virt)) PageTableLevel<level>;

			return *reinterpret_cast<PageTableLevel<level>*>(virt);
		}

		template<unsigned int level> void PageTableLevel<level>::Destroy(void)
		{
			static_assert(level > 0, "Top level page table cannot be destroyed.");
			static_assert(level < PAGE_LEVELS, "Table level exceeds number of paging levels.");

			Memory::FreeBlock<Memory::PGB_4K>(this);
		}

		inline PageTableLevel<0>& PageTableTop(void)
		{
			return PageTableLevel<0>::Table(0);
		}

#ifdef ARCH_X86_IA32
#ifdef CONFIG_PAE
		typedef PageTableLevel<0> PageDirPtr;
		typedef PageTableLevel<1> PageDir;
		typedef PageTableLevel<2> PageTab;
#else
		typedef PageTableLevel<0> PageDir;
		typedef PageTableLevel<1> PageTab;
#endif
#endif
#ifdef ARCH_X86_AMD64
		typedef PageTableLevel<0> PagePml4;
		typedef PageTableLevel<1> PageDirPtr;
		typedef PageTableLevel<2> PageDir;
		typedef PageTableLevel<3> PageTab;
#endif

		template<Memory::PageBits bits> constexpr unsigned int PageSizeLevel = ~0;

#ifdef ARCH_X86_IA32
#ifdef CONFIG_PAE
		template<> constexpr unsigned int PageSizeLevel<Memory::PGB_4K> = 2;
		template<> constexpr unsigned int PageSizeLevel<Memory::PGB_2M> = 1;
#else
		template<> constexpr unsigned int PageSizeLevel<Memory::PGB_4K> = 1;
		template<> constexpr unsigned int PageSizeLevel<Memory::PGB_4M> = 0;
#endif
#endif
#ifdef ARCH_X86_AMD64
		template<> constexpr unsigned int PageSizeLevel<Memory::PGB_4K> = 3;
		template<> constexpr unsigned int PageSizeLevel<Memory::PGB_2M> = 2;
		template<> constexpr unsigned int PageSizeLevel<Memory::PGB_1G> = 1;
#endif
	}
}

#endif

