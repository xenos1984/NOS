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

		template<> uintptr_t TryMapPage<Memory::MinPageBits>(Memory::PhysAddr phys, uintptr_t virt, [[maybe_unused]] uintptr_t length, Memory::MemType type)
		{
			MapPage<Memory::MinPageBits>(phys, virt, type);
			return Memory::MinPageSize;
		}

		bool Map(Memory::PhysAddr phys, uintptr_t virt, size_t length, Memory::MemType type)
		{
			uintptr_t diff;

//			Console::WriteFormat("Map 0x%8x bytes from 0x%8x to 0x%8x.\n", length, phys, virt);

			// Check alignment of supplied memory locations.
			if(phys & Memory::MinPageMask)
				return false;

			if(virt & Memory::MinPageMask)
				return false;

			if(length & Memory::MinPageMask)
				return false;

			// Check whether there is anything already mapped in that area.
/*			if(!IsUnmapped(virt, length))
				return false;
*/
			// Now that everything is checked, we can actually start mapping.
			for(diff = 0; diff < length; )
				diff += TryMapPage<Memory::PGB_1G>(phys + diff, virt + diff, length - diff, type);

			return true;
		}
	}
}
