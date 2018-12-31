// Pager.cpp - Working with page tables.

#include <cstdint>
#include <new>
#include <Pager.h>
#include <Symbol.h>
#include <Console.h>
#include INC_SUBARCH(PageTable.h)
#include INC_BITS(SystemRegs.h)

namespace Kernel
{
	namespace Pager
	{
		Memory::PageBits MappedSize(uintptr_t virt)
		{
			bool kernel;
			unsigned int tab, entry;

			if(virt >= MinKernelVirt)
				kernel = true;
			else if(virt <= MaxUserVirt)
				kernel = false;
			else
				return Memory::PGB_INV;

			virt &= (1ULL << (64 - PageSizeOffset)) - 1;

			if constexpr(InitialLookupLevel == 0)
			{
				entry = virt >> (4 * GranuleSize - 9);
				PageTableEntry& pte0 = PageTableLevel<0>::Table(kernel, 0).Entry(entry);
				if(pte0.IsInvalid())
					return Memory::PGB_INV;
				if(pte0.IsBlock()) // No blocks at this level.
					return Memory::PGB_INV;
			}

			if constexpr(InitialLookupLevel <= 1)
			{
				entry = (virt >> (3 * GranuleSize - 6)) & ((1ULL << (GranuleSize - 3)) - 1);
				tab = virt >> (4 * GranuleSize - 9);
				PageTableEntry& pte1 = PageTableLevel<1>::Table(kernel, tab).Entry(entry);
				if(pte1.IsInvalid())
					return Memory::PGB_INV;
				if(pte1.IsBlock())
					return (Memory::PageBits)(3 * GranuleSize - 6);
			}

			if constexpr(InitialLookupLevel <= 2)
			{
				entry = (virt >> (2 * GranuleSize - 3)) & ((1ULL << (GranuleSize - 3)) - 1);
				tab = virt >> (3 * GranuleSize - 6);
				PageTableEntry& pte2 = PageTableLevel<2>::Table(kernel, tab).Entry(entry);
				if(pte2.IsInvalid())
					return Memory::PGB_INV;
				if(pte2.IsBlock())
					return (Memory::PageBits)(2 * GranuleSize - 3);
			}

			entry = (virt >> GranuleSize) & ((1ULL << (GranuleSize - 3)) - 1);
			tab = virt >> (2 * GranuleSize - 3);
			PageTableEntry& pte3 = PageTableLevel<3>::Table(kernel, tab).Entry(entry);
			if(pte3.IsPage())
				return GranuleSize;

			return Memory::PGB_INV;
		}

		template<Memory::PageBits bits> void MapPage(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type)
		{
			bool kernel;
			unsigned int tab, entry;
			constexpr int level = 4 - (bits - 3) / (GranuleSize - 3);

			static_assert(IsValidSize(bits), "invalid page size");

			if(virt >= MinKernelVirt)
				kernel = true;
			else if(virt <= MaxUserVirt)
				kernel = false;
			else
				return;

			virt &= (1ULL << (64 - PageSizeOffset)) - 1;
			tab = virt >> (bits + GranuleSize - 3);
			entry = (virt >> bits) & ((1 << (GranuleSize - 3)) - 1);

			if(!PageTableLevel<level>::Exists(kernel, tab))
				/*PageTableLevel<level>::Create(kernel, tab)*/;

			PageTableEntry& pte = PageTableLevel<level>::Table(kernel, tab).Entry(entry);
			pte.Set<bits>(phys, type);
		}

		template<Memory::PageBits bits> void UnmapPage(uintptr_t virt)
		{
			static_assert(IsValidSize(bits), "invalid page size");
		}

		template void MapPage<GranuleSize>(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type);
		template void MapPage<(Memory::PageBits)(2 * GranuleSize - 3)>(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type);
		template void MapPage<(Memory::PageBits)(3 * GranuleSize - 6)>(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type);

		template void UnmapPage<GranuleSize>(uintptr_t virt);
		template void UnmapPage<(Memory::PageBits)(2 * GranuleSize - 3)>(uintptr_t virt);
		template void UnmapPage<(Memory::PageBits)(3 * GranuleSize - 6)>(uintptr_t virt);

		Memory::PhysAddr VirtToPhys(uintptr_t addr)
		{
			bool kernel;
			unsigned int tab, entry;

			//Console::WriteFormat("VirtToPhys(%p)\n", addr);

			if(addr >= MinKernelVirt)
				kernel = true;
			else if(addr <= MaxUserVirt)
				kernel = false;
			else
				return ~0;

			addr &= (1ULL << (64 - PageSizeOffset)) - 1;

			if constexpr(InitialLookupLevel == 0)
			{
				entry = addr >> (4 * GranuleSize - 9);
				PageTableEntry& pte0 = PageTableLevel<0>::Table(kernel, 0).Entry(entry);
				if(pte0.IsInvalid())
					return ~0;
				if(pte0.IsBlock()) // No blocks at this level.
					return ~0;
			}

			if constexpr(InitialLookupLevel <= 1)
			{
				entry = (addr >> (3 * GranuleSize - 6)) & ((1ULL << (GranuleSize - 3)) - 1);
				tab = addr >> (4 * GranuleSize - 9);
				PageTableEntry& pte1 = PageTableLevel<1>::Table(kernel, tab).Entry(entry);
				if(pte1.IsInvalid())
					return ~0;
				if(pte1.IsBlock())
					return pte1.Phys() | (addr & ((1ULL << (3 * GranuleSize - 6)) - 1));
			}

			if constexpr(InitialLookupLevel <= 2)
			{
				entry = (addr >> (2 * GranuleSize - 3)) & ((1ULL << (GranuleSize - 3)) - 1);
				tab = addr >> (3 * GranuleSize - 6);
				PageTableEntry& pte2 = PageTableLevel<2>::Table(kernel, tab).Entry(entry);
				if(pte2.IsInvalid())
					return ~0;
				if(pte2.IsBlock())
					return pte2.Phys() | (addr & ((1ULL << (2 * GranuleSize - 3)) - 1));
			}

			entry = (addr >> GranuleSize) & ((1ULL << (GranuleSize - 3)) - 1);
			tab = addr >> (2 * GranuleSize - 3);
			PageTableEntry& pte3 = PageTableLevel<3>::Table(kernel, tab).Entry(entry);
			if(pte3.IsPage())
				return pte3.Phys() | (addr & ((1ULL << GranuleSize) - 1));

			return ~0;
		}
	}
}
