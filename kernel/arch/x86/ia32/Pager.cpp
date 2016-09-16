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

		template<> PageTableEntry& PageTable<Memory::PGB_4M>(uintptr_t virt)
		{
			return ((PageTableEntry*)REC_PAGE_DIR)[virt >> Memory::PGB_4M];
		}
*/
		template<> bool MapPage<Memory::PGB_4K>(Memory::PhysAddr phys, uintptr_t virt, unsigned int flags)
		{
			if(phys & Memory::PGM_4K)
				return false;
			if(virt & Memory::PGM_4K)
				return false;

			unsigned int tab = virt >> Memory::PGB_4M;
			unsigned int entry = (virt >> Memory::PGB_4K) & 0x3ff;

			if(!PageTable32::Exists<1>(tab))
				PageTable32::Create<1>(tab);

			PageTableEntry& pte = PageTable32::Table<1>(tab).Entry(entry);

			if(pte.IsPresent())
				return false;

			pte = phys;
			return true;
		}

		template<> bool MapPage<Memory::PGB_4M>(Memory::PhysAddr phys, uintptr_t virt, unsigned int flags)
		{
			if(phys & Memory::PGM_4M)
				return false;
			if(virt & Memory::PGM_4M)
				return false;

			PageTableEntry& pte = PageTable32::Table<0>(0).Entry(virt >> Memory::PGB_4M);

			if(pte.IsPresent())
				return false;

			pte = phys;
			return true;
/*
			pte = phys | PAGE_PRESENT | PAGE_GLOBAL | PAGE_LARGE;
*/		}
/*
		template<> bool PageDirectory::UnmapPage<Memory::PGB_4K>(uintptr_t virt)
		{
		}

		template<> bool PageDirectory::UnmapPage<Memory::PGB_4M>(uintptr_t virt)
		{
		}
*/
		bool Map(Memory::PhysAddr phys, uintptr_t virt, size_t length, unsigned long type)
		{
			uintptr_t addr, diff;
			uintptr_t end = virt + length;

			// Check alignment of supplied memory locations and length.
			if(phys & Memory::PGM_4K)
				return false;
			if(virt & Memory::PGM_4K)
				return false;
			if(length & Memory::PGM_4K)
				return false;

			// Check whether there is anything already mapped in that area.
			for(addr = virt; addr < end; )
			{
				// Should we map the whole 4MB following addr?
				if((addr & Memory::PGM_4M) == 0 && end - addr >= Memory::PGS_4M)
				{
					// If there is a page table or 4MB page mapped here, return.
					if(PageTable32::Table<0>(0).Entry(addr >> Memory::PGB_4M).IsPresent())
						return false;

					addr += Memory::PGS_4M;
				}
				else
				{
					unsigned int tab = addr >> Memory::PGB_4M;
					unsigned int entry = (addr >> Memory::PGB_4K) & 0x3ff;

					// Check whether page table exists here.
					// TODO: Check also for 4MB page already mapped here!
					if(PageTable32::Exists<1>(tab))
					{
						// Check whether this entry is already mapped.
						PageTableEntry& pte = PageTable32::Table<1>(tab).Entry(entry);
						if(pte.IsPresent())
							return false;

						addr += Memory::PGS_4K;
					}
					else
						// If there is no page table here at all, we can advance to the next page table immediately.
						addr = (tab + 1) << Memory::PGB_4M;
				}
			}

			// Now that everything is checked, we can actually start mapping.
			for(diff = 0; diff < length; )
			{
				// Should we map the whole 4MB following addr?
				if(((virt + diff) & Memory::PGM_4M) == 0 && length - diff >= Memory::PGS_4M)
				{
					MapPage<Memory::PGB_4M>(phys + diff, virt + diff, type);
					diff += Memory::PGS_4M;
				}
				else
				{
					MapPage<Memory::PGB_4K>(phys + diff, virt + diff, type);
					diff += Memory::PGS_4K;
				}
			}

			return true;
		}

		void Test(void)
		{
			MapPage<Memory::PGB_4K>(0x1000000, 0x2000000, 0);
			MapPage<Memory::PGB_4M>(0x1000000, 0x00000000, 0);
		}
	}
}
