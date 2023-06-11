// Pager.h - Working with page tables.

#ifndef __PAGER_H__
#define __PAGER_H__

#include <cstddef>
#include <bit>
#include <Memory.h>

namespace Kernel
{
	namespace Pager
	{
		/** Map a single physical page to a virtual address and assign page table flags suitable for a given memory type. */
		template<Memory::PageBits bits> void MapPage(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type);

		/** Unmap a page from a virtual address. */
		template<Memory::PageBits bits> void UnmapPage(uintptr_t virt);

		/** Check the size of a page mapped to a given address. */
		Memory::PageBits MappedSize(uintptr_t virt);

		/** Check whether a particular physical memory range is completely mapped. */
		bool IsMapped(uintptr_t virt, size_t length);

		/** Check whether a particular physical memory range is completely unmapped. */
		bool IsUnmapped(uintptr_t virt, size_t length);

		/** Map a range of physical to virtual memory and and assign page table flags suitable for a given memory type. */
		bool Map(Memory::PhysAddr phys, uintptr_t virt, size_t length, Memory::MemType type);

		/** Unmap a range of virtual memory. */
		bool Unmap(uintptr_t virt, size_t length);

		/** Get physical address to which a virtual address is mapped. */
		Memory::PhysAddr VirtToPhys(uintptr_t virt);
		inline Memory::PhysAddr VirtToPhys(void* virt) { return VirtToPhys((uintptr_t)virt); }
	}
}

#include INC_ARCH(Pager.h)

namespace Kernel
{
	namespace Pager
	{
		/** Indicates whether pages of this size can be mapped / unmapped by the pager. */
		constexpr bool IsValidSize(Memory::PageBits size)
		{
			return Memory::IsValidSize(size, ValidSizes);
		}

		static const Memory::PageBits MinPageBits = static_cast<Memory::PageBits>(std::countr_zero(ValidSizes));
		static const Memory::PageBits MaxPageBits = static_cast<Memory::PageBits>(std::bit_width(ValidSizes) - 1);

		static const Memory::PageSize MinPageSize = static_cast<Memory::PageSize>(1ULL << MinPageBits);
		static const Memory::PageSize MaxPageSize = static_cast<Memory::PageSize>(1ULL << MaxPageBits);

		static const Memory::PageMask MinPageMask = static_cast<Memory::PageMask>(MinPageSize - 1);
		static const Memory::PageMask MaxPageMask = static_cast<Memory::PageMask>(MaxPageSize - 1);
	}
}

#endif
