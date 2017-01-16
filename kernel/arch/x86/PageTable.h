// PageTable.h - x86 page table class.

#ifndef __ARCH_X86_PAGETABLE_H__
#define __ARCH_X86_PAGETABLE_H__

#include <type_traits>
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
		template<unsigned int size> class PageTableSize
		{
		private:
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
			/** Return i if this is the i'th table at this level. */
			inline unsigned long Index(void);
			/** Reference to the i'th table at this level. */
			static inline PageTableLevel<level>& Table(unsigned long i);

			/** Page table which contains the page table entry pointing to this page table. */
			inline PageTableLevel<level - 1>& Parent(void);
			/** Page table entry which points to this page table. */
			inline PageTableEntry& Pointer(void);

			/** Check whether this table exists (top level table always exists). */
//			template<typename std::enable_if<level == 0>::type* = nullptr> static bool Exists(unsigned long i);
			/** Check whether this table exists. */
//			template<typename std::enable_if<level != 0>::type* = nullptr> static bool Exists(unsigned long i);

			/** Create new page table at this level. */
			static PageTableLevel<level>& Create(unsigned long i);
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

		template<unsigned int level> PageTableLevel<level>& PageTableLevel<level>::Create(unsigned long i)
		{
			static_assert(level > 0, "Top level page table cannot be created.");
			static_assert(level < PAGE_LEVELS, "Table level exceeds number of paging levels.");

			Memory::PhysAddr phys = Chunker::Alloc();
			uintptr_t virt = PAGE_TABLE_ADDR[level + 1] + i * sizeof(PageTableLevel<level>);

			Pager::MapPage<Memory::PGB_4K>(phys, virt, Memory::MemType::KERNEL_RW);
			new (reinterpret_cast<PageTableLevel<level>*>(virt)) PageTableLevel<level>;

			return *reinterpret_cast<PageTableLevel<level>*>(virt);
		}

		template<unsigned int level> void PageTableLevel<level>::Destroy(void)
		{
			static_assert(level > 0, "Top level page table cannot be destroyed.");
			static_assert(level < PAGE_LEVELS, "Table level exceeds number of paging levels.");

			Memory::PhysAddr phys = Pointer().Phys();
			Pager::UnmapPage<Memory::PGB_4K>((uintptr_t)this);
			Chunker::Free(phys);
		}

		inline PageTableLevel<0>& PageTableTop(void)
		{
			return PageTableLevel<0>::Table(0);
		}

		template<unsigned int size> class PageTable
		{
		private:
			PageTableEntry entry[size] = {PageTableEntry{}};

		public:
			inline PageTableEntry& Entry(unsigned int i);
			template<unsigned int level> inline unsigned long Index(void);
			template<unsigned int level> static inline PageTable<size>& Table(unsigned long i);
			static inline PageTable<size>& Top(void);
			template<unsigned int level, typename std::enable_if<level == 0>::type* = nullptr> static bool Exists(unsigned long i);
			template<unsigned int level, typename std::enable_if<level != 0>::type* = nullptr> static bool Exists(unsigned long i);
			bool IsEmpty(void);
			template<unsigned int level> static PageTable<size>& Create(unsigned long i);
			template<unsigned int level> void Destroy(void);
		} PACKED;

		template<unsigned int size> inline PageTableEntry& PageTable<size>::Entry(unsigned int i)
		{
			return entry[i];
		}

		template<unsigned int size> template<unsigned int level> inline unsigned long PageTable<size>::Index(void)
		{
			static_assert(level < PAGE_LEVELS, "Table level exceeds number of paging levels.");
			return (reinterpret_cast<uintptr_t>(this) - PAGE_TABLE_ADDR[level + 1]) / sizeof(PageTable<size>);
		}

		template<unsigned int size> template<unsigned int level> inline PageTable<size>& PageTable<size>::Table(unsigned long i)
		{
			static_assert(level < PAGE_LEVELS, "Table level exceeds number of paging levels.");
			return reinterpret_cast<PageTable<size>*>(PAGE_TABLE_ADDR[level + 1])[i];
		}

		template<unsigned int size> inline PageTable<size>& PageTable<size>::Top(void)
		{
			return Table<0>(0);
		}

		template<unsigned int size> template<unsigned int level, typename std::enable_if<level == 0>::type*> bool PageTable<size>::Exists(unsigned long i __attribute__((unused)))
		{
			return true;
		}

		template<unsigned int size> template<unsigned int level, typename std::enable_if<level != 0>::type*> bool PageTable<size>::Exists(unsigned long i)
		{
			static_assert(level < PAGE_LEVELS, "Table level exceeds number of paging levels.");

			if(!Exists<level - 1>(i >> PAGE_BITS[level]))
				return false;

			return Table<level - 1>(i >> PAGE_BITS[level]).Entry(i & ((1 << PAGE_BITS[level]) - 1)).IsPresent();
		}

		template<unsigned int size> bool PageTable<size>::IsEmpty(void)
		{
			for(unsigned int i = 0; i < size; i++)
			{
				if(!entry[i].IsClear())
					return false;
			}

			return true;
		}

		template<unsigned int size> template<unsigned int level> PageTable<size>& PageTable<size>::Create(unsigned long i)
		{
			static_assert(level > 0, "Top level page table cannot be created.");
			static_assert(level < PAGE_LEVELS, "Table level exceeds number of paging levels.");

			Memory::PhysAddr phys = Chunker::Alloc();
			uintptr_t virt = PAGE_TABLE_ADDR[level + 1] + i * sizeof(PageTable<size>);

			Pager::MapPage<Memory::PGB_4K>(phys, virt, Memory::MemType::KERNEL_RW);
			new (reinterpret_cast<PageTable*>(virt)) PageTable;

			return *reinterpret_cast<PageTable*>(virt);
		}

		template<unsigned int size> template<unsigned int level> void PageTable<size>::Destroy(void)
		{
			static_assert(level > 0, "Top level page table cannot be destroyed.");
			static_assert(level < PAGE_LEVELS, "Table level exceeds number of paging levels.");

			unsigned long idx = this->Index<level>();
			Memory::PhysAddr phys = Top().Entry(idx).Phys();
			Chunker::Free(phys);
			Pager::UnmapPage<Memory::PGB_4K>((uintptr_t)this);
		}

#ifdef ARCH_X86_IA32
#ifdef CONFIG_PAE
		typedef PageTableLevel<0> PageDirPtr;
		typedef PageTableLevel<1> PageDir;
		typedef PageTableLevel<2> PageTab;
		typedef PageTable<4> PageTablePtr;
		typedef PageTable<512> PageTablePAE;
#else
		typedef PageTableLevel<0> PageDir;
		typedef PageTableLevel<1> PageTab;
		typedef PageTable<1024> PageTable32;
#endif
#endif
#ifdef ARCH_X86_AMD64
		typedef PageTableLevel<0> PagePml4;
		typedef PageTableLevel<1> PageDirPtr;
		typedef PageTableLevel<2> PageDir;
		typedef PageTableLevel<3> PageTab;
		typedef PageTable<512> PageTable64;
#endif
	}
}

#endif

