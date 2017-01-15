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
		static const unsigned int PAGE_LEVELS = 3;
		static const unsigned int PAGE_BITS[] = {2, 9, 9};
		static const uintptr_t PAGE_TABLE_ADDR[] = {0xfffffff8, 0xffffffe0, 0xffffc000, 0xff800000};
#else
		static const unsigned int PAGE_LEVELS = 2;
		static const unsigned int PAGE_BITS[] = {10, 10};
		static const uintptr_t PAGE_TABLE_ADDR[] = {0xfffffffc, 0xfffff000, 0xffc00000};
#endif
#endif
#ifdef ARCH_X86_AMD64
		static const unsigned int PAGE_LEVELS = 4;
		static const unsigned int PAGE_BITS[] = {9, 9, 9, 9};
		static const uintptr_t PAGE_TABLE_ADDR[] = {0xffffff7fbfdfeff0, 0xffffff7fbfdfe000, 0xffffff7fbfc00000, 0xffffff7f80000000, 0xffffff0000000000};
#endif

		template<unsigned int size> class PageTable
		{
		private:
			PageTableEntry entry[size] = {PageTableEntry{}};

		public:
			inline PageTableEntry& Entry(unsigned int i);
			template<unsigned int level> static inline PageTable<size>& Table(unsigned long i);
			static inline PageTable<size>& Top(void);
			template<unsigned int level, typename std::enable_if<level == 0>::type* = nullptr> static bool Exists(unsigned long i);
			template<unsigned int level, typename std::enable_if<level != 0>::type* = nullptr> static bool Exists(unsigned long i);
			template<unsigned int level> bool IsEmpty(void);
			template<unsigned int level> static PageTable<size>& Create(unsigned long i);
			template<unsigned int level> static void Destroy(unsigned long i);
		} PACKED;

		template<unsigned int size> inline PageTableEntry& PageTable<size>::Entry(unsigned int i)
		{
			return entry[i];
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

		template<unsigned int size> template<unsigned int level> bool PageTable<size>::IsEmpty(void)
		{
			static_assert(level < PAGE_LEVELS, "Table level exceeds number of paging levels.");

			for(unsigned long i = 0; i < size; i++)
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

		template<unsigned int size> template<unsigned int level> void PageTable<size>::Destroy(unsigned long i)
		{
			static_assert(level > 0, "Top level page table cannot be created.");
			static_assert(level < PAGE_LEVELS, "Table level exceeds number of paging levels.");
		}

#ifdef ARCH_X86_IA32
#ifdef CONFIG_PAE
		typedef PageTable<4> PageTablePtr;
		typedef PageTable<512> PageTablePAE;
#else
		typedef PageTable<1024> PageTable32;
#endif
#endif
#ifdef ARCH_X86_AMD64
		typedef PageTable<512> PageTable64;
#endif
	}
}

#endif

