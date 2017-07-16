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
	{/*
		template<> void MapPage<Memory::PGB_4K>(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type)
		{
		}

		template<> void MapPage<Memory::PGB_2M>(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type)
		{
		}

		template<> void MapPage<Memory::PGB_1G>(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type)
		{
		}

		template<> void UnmapPage<Memory::PGB_4K>(uintptr_t virt)
		{
			unsigned int tab = (virt & 0xffffffffffffUL) >> Memory::PGB_2M;
			unsigned int entry = (virt >> Memory::PGB_4K) & 0x1ff;

			PageTab& pt = PageTab::Table(tab);
			PageTableEntry& pte = pt.Entry(entry);
			pte.Clear();
			Invalidate(virt);

			if(pt.IsEmpty())
				pt.Destroy();
		}

		template<> void UnmapPage<Memory::PGB_2M>(uintptr_t virt)
		{
			unsigned int tab = (virt & 0xffffffffffffUL) >> Memory::PGB_1G;
			unsigned int entry = (virt >> Memory::PGB_2M) & 0x1ff;

			PageDir& pt = PageDir::Table(tab);
			PageTableEntry& pte = pt.Entry(entry);
			pte.Clear();
			Invalidate(virt);

			if(pt.IsEmpty())
				pt.Destroy();
		}

		template<> void UnmapPage<Memory::PGB_1G>(uintptr_t virt)
		{
			unsigned int tab = (virt & 0xffffffffffffUL) >> Memory::PGB_512G;
			unsigned int entry = (virt >> Memory::PGB_1G) & 0x1ff;

			PageDirPtr& pt = PageDirPtr::Table(tab);
			PageTableEntry& pte = pt.Entry(entry);
			pte.Clear();
			Invalidate(virt);

			if(pt.IsEmpty())
				pt.Destroy();
		}
*/
		template void MapPage<Memory::PGB_4K>(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type);
		template void UnmapPage<Memory::PGB_4K>(uintptr_t virt);
		template void MapPage<Memory::PGB_2M>(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type);
		template void UnmapPage<Memory::PGB_2M>(uintptr_t virt);
		template void MapPage<Memory::PGB_1G>(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type);
		template void UnmapPage<Memory::PGB_1G>(uintptr_t virt);

		Memory::PageBits MappedSize(uintptr_t virt)
		{
			// Normalize address to smallest possible page size and remove sign extension.
			virt &= 0x0000ffffffffffff & ~Memory::PGM_4K;

			// Check top level page table first.
			PageTableEntry& pte512g = PageTableTop().Entry(virt >> Memory::PGB_512G);

			// If it is not present, then this page is not mapped.
			if(!pte512g.IsPresent())
				return Memory::PGB_INV;

			// Check next level
			PageTableEntry& pte1g = PageDirPtr::Table(virt >> Memory::PGB_512G).Entry((virt >> Memory::PGB_1G) & 0x1ff);

			// If it is not present, then this page is not mapped.
			if(!pte1g.IsPresent())
				return Memory::PGB_INV;

			// If it is present and 1GB, then this page is mapped.
			if(pte1g.IsLarge())
				return Memory::PGB_1G;

			// For a smaller page, we need to check the next level.
			PageTableEntry& pte2m = PageDir::Table(virt >> Memory::PGB_1G).Entry((virt >> Memory::PGB_2M) & 0x1ff);

			// If it is not present, then this page is not mapped.
			if(!pte2m.IsPresent())
				return Memory::PGB_INV;

			// If it is present and 4MB, then this page is mapped.
			if(pte2m.IsLarge())
				return Memory::PGB_2M;

			// For a 4kB page, we need to check the next level.
			PageTableEntry& pte4k = PageTab::Table(virt >> Memory::PGB_2M).Entry((virt >> Memory::PGB_4K) & 0x1ff);

			// If it is present, this page is mapped.
			if(pte4k.IsPresent())
				return Memory::PGB_4K;

			// Otherwise, it is not mapped.
			return Memory::PGB_INV;
		}

		bool IsMapped(uintptr_t virt, size_t length)
		{
			virt &= 0x0000ffffffffffff; // Remove sign extension.

			unsigned int plv0, plv1, plv2, plv3;
			uintptr_t addr;
			uintptr_t end = virt + length;

			// Check alignment.
			if(virt & Memory::PGM_4K)
				return false;
			if(length & Memory::PGM_4K)
				return false;

			// Go through all pages.
			for(addr = virt; addr < end; )
			{
				plv0 = addr >> Memory::PGB_512G;

				// If page directory pointer is not present, then this address is not mapped.
				if(!PageTableTop().Entry(plv0).IsPresent())
					return false;

				// Check entries at 1GB level.
				PageDirPtr& pt1 = PageDirPtr::Table(plv0);
				do
				{
					plv1 = (addr >> Memory::PGB_1G) & 0x1ff;

					// Check whether 1GB entry is present.
					if(!pt1.Entry(plv1).IsPresent())
						return false;

					// If there is a large page, then everything up to the next 1GB is mapped.
					if(pt1.Entry(plv1).IsLarge())
					{
						addr = (addr + Memory::PGS_1G) & ~Memory::PGM_1G;
						continue;
					}

					// Otherwise, check for 2MB page mappings.
					PageDir& pt2 = PageDir::Table(plv1);
					do
					{
						plv2 = (addr >> Memory::PGB_2M) & 0x1ff;

						// Check whether 2MB entry is present.
						if(!pt2.Entry(plv2).IsPresent())
							return false;

						// If there is a large page, then everything up to the next 2MB is mapped.
						if(pt2.Entry(plv2).IsLarge())
						{
							addr = (addr + Memory::PGS_2M) & ~Memory::PGM_2M;
							continue;
						}

						// Otherwise, check for 4kB page mappings.
						PageTab& pt3 = PageTab::Table(plv2);
						do
						{
							plv3 = (addr >> Memory::PGB_4K) & 0x1ff;

							// Check whether this page is mapped.
							if(!pt3.Entry(plv3).IsPresent())
								return false;

							addr += Memory::PGS_4K;
						}
						while((addr < end) && (plv3 < 0x200));
					}
					while((addr < end) && (plv2 < 0x200));
				}
				while((addr < end) && (plv1 < 0x200));
			}

			return true;
		}

		bool IsUnmapped(uintptr_t virt, size_t length)
		{
			virt &= 0x0000ffffffffffff; // Remove sign extension.

			unsigned int plv0, plv1, plv2, plv3;
			uintptr_t addr;
			uintptr_t end = virt + length;

			// Check alignment.
			if(virt & Memory::PGM_4K)
				return false;
			if(length & Memory::PGM_4K)
				return false;

			// Go through all pages.
			for(addr = virt; addr < end; )
			{
				plv0 = addr >> Memory::PGB_512G;

				// If top level entry does not exist, we can advance by 512GB.
				if(!PageTableTop().Entry(plv0).IsPresent())
				{
					addr += Memory::PGS_512G;
					continue;
				}

				// Should we check the whole 512GB following addr?
				if((addr & Memory::PGM_512G) == 0 && end - addr >= Memory::PGS_512G)
					return false;

				// Check entries at 1GB level.
				PageDirPtr& pt1 = PageDirPtr::Table(plv0);
				do
				{
					plv1 = (addr >> Memory::PGB_1G) & 0x1ff;

					// If entry does not exist, we can advance by 1GB.
					if(!pt1.Entry(plv1).IsPresent())
					{
						addr += Memory::PGS_1G;
						continue;
					}

					// If it is large, then this address is mapped.
					if(pt1.Entry(plv1).IsLarge())
						return false;

					// Should we check the whole 1GB following addr?
					if((addr & Memory::PGM_1G) == 0 && end - addr >= Memory::PGS_1G)
						return false;

					// Check entries at 2MB level.
					PageDir& pt2 = PageDir::Table(plv1);
					do
					{
						plv2 = (addr >> Memory::PGB_2M) & 0x1ff;

						// If entry does not exist, we can advance by 2MB.
						if(!pt2.Entry(plv2).IsPresent())
						{
							addr += Memory::PGS_2M;
							continue;
						}

						// If it is large, then this address is mapped.
						if(pt2.Entry(plv2).IsLarge())
							return false;

						// Should we check the whole 2MB following addr?
						if((addr & Memory::PGM_2M) == 0 && end - addr >= Memory::PGS_2M)
							return false;

						// Check entries at 4kB level.
						PageTab& pt3 = PageTab::Table(plv2);
						do
						{
							plv3 = (addr >> Memory::PGB_4K) & 0x1ff;

							// Check whether this entry is already mapped.
							if(pt3.Entry(plv3).IsPresent())
								return false;

							addr += Memory::PGS_4K;
						}
						while((addr < end) && (plv3 < 0x200));
					}
					while((addr < end) && (plv2 < 0x200));
				}
				while((addr < end) && (plv1 < 0x200));
			}

			return true;
		}

		bool Map(Memory::PhysAddr phys, uintptr_t virt, size_t length, Memory::MemType type)
		{
			uintptr_t diff;

//			Console::WriteFormat("Map 0x%8x bytes from 0x%8x to 0x%8x.\n", length, phys, virt);

			// Check alignment of supplied memory location.
			if(phys & Memory::PGM_4K)
				return false;

			// Check whether there is anything already mapped in that area.
			if(!IsUnmapped(virt, length))
				return false;

			// Now that everything is checked, we can actually start mapping.
			for(diff = 0; diff < length; )
			{
				// Should we map the whole 1GBB following addr?
				if(((virt + diff) & Memory::PGM_1G) == 0 && length - diff >= Memory::PGS_1G)
				{
					MapPage<Memory::PGB_1G>(phys + diff, virt + diff, type);
					diff += Memory::PGS_1G;
				}
				// Should we map the whole 2MB following addr?
				else if(((virt + diff) & Memory::PGM_2M) == 0 && length - diff >= Memory::PGS_2M)
				{
					MapPage<Memory::PGB_2M>(phys + diff, virt + diff, type);
					diff += Memory::PGS_2M;
				}
				else
				{
					MapPage<Memory::PGB_4K>(phys + diff, virt + diff, type);
					diff += Memory::PGS_4K;
				}
			}

			return true;
		}

		bool Unmap(uintptr_t virt, size_t length)
		{
			uintptr_t addr;
			uintptr_t end = virt + length;

			// Check whether this area is indeed mapped.
			if(!IsMapped(virt, length))
				return false;

			for(addr = virt; addr < end; )
			{
				switch(MappedSize(addr))
				{
				case Memory::PGB_4K:
					UnmapPage<Memory::PGB_4K>(addr);
					addr += Memory::PGS_4K;
					break;
				case Memory::PGB_2M:
					UnmapPage<Memory::PGB_2M>(addr);
					addr += Memory::PGS_2M;
					break;
				case Memory::PGB_1G:
					UnmapPage<Memory::PGB_1G>(addr);
					addr += Memory::PGS_1G;
					break;
				default:
					return false;
				}
			}

			return true;
		}

		Memory::PhysAddr VirtToPhys(uintptr_t addr)
		{
			addr &= 0x0000ffffffffffff; // Remove sign extension.

			PageTableEntry& pte0 = PageTableTop().Entry(addr >> Memory::PGB_512G);
			if(!pte0.IsPresent())
				return ~0;

			PageTableEntry& pte1 = PageDirPtr::Table(addr >> Memory::PGB_512G).Entry((addr >> Memory::PGB_1G) & 0x1ff);
			if(!pte1.IsPresent())
				return ~0;
			if(pte1.IsLarge())
				return pte1.Phys() | (addr & Memory::PGM_1G);

			PageTableEntry& pte2 = PageDirPtr::Table(addr >> Memory::PGB_1G).Entry((addr >> Memory::PGB_2M) & 0x1ff);
			if(!pte2.IsPresent())
				return ~0;
			if(pte2.IsLarge())
				return pte2.Phys() | (addr & Memory::PGM_2M);

			PageTableEntry& pte3 = PageDirPtr::Table(addr >> Memory::PGB_2M).Entry((addr >> Memory::PGB_4K) & 0x1ff);
			if(!pte3.IsPresent())
				return ~0;

			return pte3.Phys() | (addr & Memory::PGM_4K);
		}
	}
}
