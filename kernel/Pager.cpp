// Pager.cpp - Working with page tables.

#include <Pager.h>
#include <Memory.h>
#include <type_traits>

namespace Kernel
{
	namespace Pager
	{
		template<Memory::PageBits size> uintptr_t TryMapPage(Memory::PhysAddr phys, uintptr_t virt, uintptr_t length, typename std::enable_if<!IsValidSize(size), Memory::MemType>::type type);
		template<Memory::PageBits size> uintptr_t TryMapPage(Memory::PhysAddr phys, uintptr_t virt, uintptr_t length, typename std::enable_if<IsValidSize(size), Memory::MemType>::type type);

		template<Memory::PageBits size> uintptr_t TryMapPage(Memory::PhysAddr phys, uintptr_t virt, uintptr_t length, typename std::enable_if<!IsValidSize(size), Memory::MemType>::type type)
		{
			return TryMapPage<(Memory::PageBits)(size - 1)>(phys, virt, length, type);
		}

		template<Memory::PageBits size> uintptr_t TryMapPage(Memory::PhysAddr phys, uintptr_t virt, uintptr_t length, typename std::enable_if<IsValidSize(size), Memory::MemType>::type type)
		{
			if((((virt | phys) & ((1 << size) - 1)) == 0) && (length >= (1 << size)))
			{
				MapPage<size>(phys, virt, type);
				return (1 << size);
			}
			else
				return TryMapPage<(Memory::PageBits)(size - 1)>(phys, virt, length, type);
		}

		template<> uintptr_t TryMapPage<Pager::MinPageBits>(Memory::PhysAddr phys, uintptr_t virt, [[maybe_unused]] uintptr_t length, Memory::MemType type)
		{
			MapPage<Pager::MinPageBits>(phys, virt, type);
			return Pager::MinPageSize;
		}

		bool Map(Memory::PhysAddr phys, uintptr_t virt, size_t length, Memory::MemType type)
		{
			uintptr_t diff;

//			Console::WriteFormat("Map 0x%8x bytes from 0x%8x to 0x%8x.\n", length, phys, virt);

			// Check alignment of supplied memory locations.
			if(phys & Pager::MinPageMask)
				return false;

			if(virt & Pager::MinPageMask)
				return false;

			if(length & Pager::MinPageMask)
				return false;

			// Check whether there is anything already mapped in that area.
/*			if(!IsUnmapped(virt, length))
				return false;
*/
			// Now that everything is checked, we can actually start mapping.
			for(diff = 0; diff < length; )
				diff += TryMapPage<Pager::MaxPageBits>(phys + diff, virt + diff, length - diff, type);

			return true;
		}

		template<Memory::PageBits size> void TryUnmapPage(uintptr_t addr, typename std::enable_if<!IsValidSize(size), Memory::PageBits>::type mapped);
		template<Memory::PageBits size> void TryUnmapPage(uintptr_t addr, typename std::enable_if<IsValidSize(size), Memory::PageBits>::type mapped);

		template<Memory::PageBits size> void TryUnmapPage(uintptr_t addr, typename std::enable_if<!IsValidSize(size), Memory::PageBits>::type mapped)
		{
			TryUnmapPage<(Memory::PageBits)(size - 1)>(addr, mapped);
		}

		template<Memory::PageBits size> void TryUnmapPage(uintptr_t addr, typename std::enable_if<IsValidSize(size), Memory::PageBits>::type mapped)
		{
			if(size == mapped)
				UnmapPage<size>(addr);
			else
				TryUnmapPage<(Memory::PageBits)(size - 1)>(addr, mapped);
		}

		template<> void TryUnmapPage<Pager::MinPageBits>(uintptr_t addr, [[maybe_unused]] Memory::PageBits mapped)
		{
			UnmapPage<Pager::MinPageBits>(addr);
		}

		bool Unmap(uintptr_t virt, size_t length)
		{
			uintptr_t addr;
			uintptr_t end = virt + length;
/*
			// Check whether this area is indeed mapped.
			if(!IsMapped(virt, length))
				return false;
*/
			for(addr = virt; addr < end; )
			{
				Memory::PageBits mapped = MappedSize(addr);
				TryUnmapPage<Pager::MaxPageBits>(addr, mapped);
				addr += (1 << mapped);
			}

			return true;
		}
	}
}
