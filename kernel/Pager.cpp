// Pager.cpp - Working with page tables.

#include <Pager.h>
#include <Memory.h>
#include <type_traits>

namespace Kernel
{
	namespace Pager
	{
		template<Memory::PageBits size> typename std::enable_if<!IsValidSize(size), uintptr_t>::type TryMapPage(Memory::PhysAddr phys, uintptr_t virt, uintptr_t length, Memory::MemType type);
		template<Memory::PageBits size> typename std::enable_if<IsValidSize(size), uintptr_t>::type TryMapPage(Memory::PhysAddr phys, uintptr_t virt, uintptr_t length, Memory::MemType type);

		template<Memory::PageBits size> typename std::enable_if<!IsValidSize(size), uintptr_t>::type TryMapPage(Memory::PhysAddr phys, uintptr_t virt, uintptr_t length, Memory::MemType type)
		{
			return TryMapPage<(Memory::PageBits)(size - 1)>(phys, virt, length, type);
		}

		template<Memory::PageBits size> typename std::enable_if<IsValidSize(size), uintptr_t>::type TryMapPage(Memory::PhysAddr phys, uintptr_t virt, uintptr_t length, Memory::MemType type)
		{
			if((((virt | phys) & ((1 << size) - 1)) == 0) && (length >= (1 << size)))
			{
				MapPage<size>(phys, virt, type);
				return (1 << size);
			}
			else
				return TryMapPage<(Memory::PageBits)(size - 1)>(phys, virt, length, type);
		}

		template<> uintptr_t TryMapPage<MinPageBits>(Memory::PhysAddr phys, uintptr_t virt, [[maybe_unused]] uintptr_t length, Memory::MemType type)
		{
			MapPage<MinPageBits>(phys, virt, type);
			return MinPageSize;
		}

		bool Map(Memory::PhysAddr phys, uintptr_t virt, size_t length, Memory::MemType type)
		{
			uintptr_t diff;

//			Console::WriteFormat("Map 0x%8x bytes from 0x%8x to 0x%8x.\n", length, phys, virt);

			// Check alignment of supplied memory locations.
			if(phys & MinPageMask)
				return false;

			if(virt & MinPageMask)
				return false;

			if(length & MinPageMask)
				return false;

			// Check whether there is anything already mapped in that area.
/*			if(!IsUnmapped(virt, length))
				return false;
*/
			// Now that everything is checked, we can actually start mapping.
			for(diff = 0; diff < length; )
				diff += TryMapPage<MaxPageBits>(phys + diff, virt + diff, length - diff, type);

			return true;
		}

		template<Memory::PageBits size> typename std::enable_if<!IsValidSize(size), void>::type TryUnmapPage(uintptr_t addr, Memory::PageBits mapped);
		template<Memory::PageBits size> typename std::enable_if<IsValidSize(size), void>::type TryUnmapPage(uintptr_t addr, Memory::PageBits mapped);

		template<Memory::PageBits size> typename std::enable_if<!IsValidSize(size), void>::type TryUnmapPage(uintptr_t addr, Memory::PageBits mapped)
		{
			TryUnmapPage<(Memory::PageBits)(size - 1)>(addr, mapped);
		}

		template<Memory::PageBits size> typename std::enable_if<IsValidSize(size), void>::type TryUnmapPage(uintptr_t addr, Memory::PageBits mapped)
		{
			if(size == mapped)
				UnmapPage<size>(addr);
			else
				TryUnmapPage<(Memory::PageBits)(size - 1)>(addr, mapped);
		}

		template<> void TryUnmapPage<MinPageBits>(uintptr_t addr, [[maybe_unused]] Memory::PageBits mapped)
		{
			UnmapPage<MinPageBits>(addr);
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
				TryUnmapPage<MaxPageBits>(addr, mapped);
				addr += (1ULL << mapped);
			}

			return true;
		}
	}
}
