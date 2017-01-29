// Pager.cpp - Working with page tables.

#include <cstdint>
#include <new>
#include <Pager.h>
#include <Memory.h>
#include <Chunker.h>
#include <Console.h>
#include INC_ARCH(PageTableEntry.h)
#include INC_ARCH(PageTable.h)
#include INC_ARCH(ControlRegisters.h)

namespace Kernel
{
	namespace Pager
	{
		inline void Invalidate(uintptr_t virt)
		{
			asm volatile ("invlpg (%0)" : : "r"(virt));
		}

		template<Memory::PageBits bits> void MapPage(Memory::PhysAddr phys __attribute__((unused)), uintptr_t virt __attribute__((unused)), Memory::MemType type __attribute__((unused)))
		{
			static_assert(IsValidSize(bits), "invalid page size");
		}

		template<Memory::PageBits bits> void UnmapPage(uintptr_t virt __attribute__((unused)))
		{
			static_assert(IsValidSize(bits), "invalid page size");
		}

		template<> void MapPage<Memory::PGB_4K>(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type)
		{
			unsigned int tab = virt >> Memory::PGB_4M;
			unsigned int entry = (virt >> Memory::PGB_4K) & 0x3ff;
			Memory::MemType tabtype;

			if(!PageTab::Exists(tab))
			{
				switch(type)
				{
				case Memory::MemType::KERNEL_EXEC:
				case Memory::MemType::KERNEL_RO:
				case Memory::MemType::KERNEL_RW:
					tabtype = Memory::MemType::KERNEL_RW;
					break;
				case Memory::MemType::USER_EXEC:
				case Memory::MemType::USER_RO:
				case Memory::MemType::USER_RW:
				case Memory::MemType::USER_COW:
				case Memory::MemType::USER_DEMAND:
					tabtype = Memory::MemType::USER_RW;
					break;
				default:
					tabtype = Memory::MemType::KERNEL_RW;
				}
				PageTab::Create(tab, tabtype);
			}

			PageTableEntry& pte = PageTab::Table(tab).Entry(entry);
			pte.Set<Memory::PGB_4K>(phys, type);
			Invalidate(virt);
		}

		template<> void MapPage<Memory::PGB_4M>(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type)
		{
			PageTableEntry& pte = PageTableTop().Entry(virt >> Memory::PGB_4M);
			pte.Set<Memory::PGB_4M>(phys, type);
			Invalidate(virt);
		}

		template<> void UnmapPage<Memory::PGB_4K>(uintptr_t virt)
		{
			unsigned int tab = virt >> Memory::PGB_4M;
			unsigned int entry = (virt >> Memory::PGB_4K) & 0x3ff;

			PageTab& pt = PageTab::Table(tab);
			PageTableEntry& pte = pt.Entry(entry);
			pte.Clear();
			Invalidate(virt);

			if(pt.IsEmpty())
				pt.Destroy();
		}

		template<> void UnmapPage<Memory::PGB_4M>(uintptr_t virt)
		{
			PageTableEntry& pte = PageTableTop().Entry(virt >> Memory::PGB_4M);
			pte.Clear();
			Invalidate(virt);
		}

		Memory::PageBits MappedSize(uintptr_t virt)
		{
			// Normalize address to smallest possible page size.
			virt &= Memory::PGM_4K;

			// Check top level page table first.
			PageTableEntry& pte4m = PageTableTop().Entry(virt >> Memory::PGB_4M);

			// If it is not present, then this page is not mapped.
			if(!pte4m.IsPresent())
				return Memory::PGB_INV;

			// If it is present and 4MB, then this page is mapped.
			if(pte4m.IsLarge())
				return Memory::PGB_4M;

			// For a 4kB page, we need to check the next level.
			PageTableEntry& pte4k = PageTab::Table(virt >> Memory::PGB_4M).Entry((virt >> Memory::PGB_4K) & 0x3ff);

			// If it is present, this page is mapped.
			if(pte4k.IsPresent())
				return Memory::PGB_4K;

			// Otherwise, it is not mapped.
			return Memory::PGB_INV;
		}

		bool Map(Memory::PhysAddr phys, uintptr_t virt, size_t length, Memory::MemType type)
		{
			uintptr_t addr, diff;
			uintptr_t end = virt + length;

//			Console::WriteFormat("Map 0x%8x bytes from 0x%8x to 0x%8x.\n", length, phys, virt);

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
				unsigned int tab = addr >> Memory::PGB_4M;
				unsigned int entry = (addr >> Memory::PGB_4K) & 0x3ff;

				// Should we map the whole 4MB following addr?
				if((addr & Memory::PGM_4M) == 0 && end - addr >= Memory::PGS_4M)
				{
					// If there is a page table or 4MB page mapped here, return.
					if(PageTableTop().Entry(tab).IsPresent())
						return false;

					addr += Memory::PGS_4M;
				}
				else
				{
					// Check whether page table exists here.
					if(PageTableTop().Entry(tab).IsPresent())
					{
						// Check whether 4MB page is mapped here.
						if(PageTableTop().Entry(tab).IsLarge())
							return false;

						// Check whether this entry is already mapped.
						if(PageTab::Table(tab).Entry(entry).IsPresent())
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
			MapPage<Memory::PGB_4K>(0x1000000, 0x2000000, Memory::MemType::KERNEL_RO);
			MapPage<Memory::PGB_4M>(0x1000000, 0x00000000, Memory::MemType::KERNEL_EXEC);
		}
	}
}
