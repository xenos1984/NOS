// Pager.cpp - Working with page tables.

#include <cstdint>
#include <new>
#include <Pager.h>
#include <Memory.h>
#include <Chunker.h>
#include INC_ARCH(PageTableEntry.h)
#include INC_ARCH(PageTable.h)
#include INC_ARCH(ControlRegisters.h)

namespace Kernel
{
	namespace Pager
	{
//		static const unsigned int PAGES_PER_TABLE = 1024;
//		static const unsigned int PAGE_RECURSIVE = 1023;
		static const unsigned int PAGE_SIZE = 4096;
		static const unsigned int PAGE_SIZE_MASK = 4095;
//		static const uintptr_t REC_PAGE_DIR = (((uintptr_t)PAGE_RECURSIVE) << 22) + (((uintptr_t)PAGE_RECURSIVE) << 12);
//		static const uintptr_t REC_PAGE_TAB = ((uintptr_t)PAGE_RECURSIVE) << 22;
/*
		template<Memory::PageBits bits> PageTableEntry& PageTable(uintptr_t virt)
		{
			static_assert(IsValidSize(bits), "invalid page size");
		}
*/
		template<Memory::PageBits bits> bool MapPage(Memory::PhysAddr phys, uintptr_t virt, unsigned int flags)
		{
			static_assert(IsValidSize(bits), "invalid page size");
			return false;
		}
/*
		template<Memory::PageBits bits> bool UnmapPage(uintptr_t virt)
		{
			static_assert(IsValidSize(bits), "invalid page size");
		}

		template<> PageTableEntry& PageTable<Memory::PAGE_4M>(uintptr_t virt)
		{
			return ((PageTableEntry*)REC_PAGE_DIR)[virt >> Memory::PAGE_4M];
		}
*/
		template<> bool MapPage<Memory::PAGE_4K>(Memory::PhysAddr phys, uintptr_t virt, unsigned int flags)
		{
			if(phys & ((1UL << Memory::PAGE_4K) - 1))
				return false;
			if(virt & ((1UL << Memory::PAGE_4K) - 1))
				return false;

			unsigned int tab = virt >> Memory::PAGE_4M;
			unsigned int entry = (virt >> Memory::PAGE_4K) & 0x3ff;

			if(!PageTable32::Exists<1>(tab))
				PageTable32::Create<1>(tab);

			PageTableEntry& pte = PageTable32::Table<1>(tab).Entry(entry);

			if(pte.IsPresent())
				return false;

			pte = phys;
			return true;
		}

		template<> bool MapPage<Memory::PAGE_4M>(Memory::PhysAddr phys, uintptr_t virt, unsigned int flags)
		{
			if(phys & ((1UL << Memory::PAGE_4M) - 1))
				return false;
			if(virt & ((1UL << Memory::PAGE_4M) - 1))
				return false;

			PageTableEntry& pte = PageTable32::Table<0>(0).Entry(virt >> Memory::PAGE_4M);

			if(pte.IsPresent())
				return false;

			pte = phys;
			return true;
/*
			pte = phys | PAGE_PRESENT | PAGE_GLOBAL | PAGE_LARGE;
*/		}
/*
		template<> bool PageDirectory::UnmapPage<Memory::PAGE_4K>(uintptr_t virt)
		{
		}

		template<> bool PageDirectory::UnmapPage<Memory::PAGE_4M>(uintptr_t virt)
		{
		}
*/
		void Test(void)
		{
			MapPage<Memory::PAGE_4K>(0x1000000, 0x2000000, 0);
			MapPage<Memory::PAGE_4M>(0x1000000, 0x00000000, 0);
		}
	}
}
