// Pager.cpp - Working with page tables.

#include <Pager.h>
#include <Memory.h>
#include INC_ARCH(ControlRegisters.h)

namespace Kernel
{
	namespace Pager
	{
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
		}

		template<> void UnmapPage<Memory::PGB_2M>(uintptr_t virt)
		{
		}

		template<> void UnmapPage<Memory::PGB_1G>(uintptr_t virt)
		{
		}

		Memory::PageBits MappedSize(uintptr_t virt)
		{
		}

		bool IsMapped(uintptr_t virt, size_t length)
		{
		}

		bool IsUnmapped(uintptr_t virt, size_t length)
		{
		}

		bool Map(Memory::PhysAddr phys, uintptr_t virt, size_t length, Memory::MemType type)
		{
		}

		bool Unmap(uintptr_t virt, size_t length)
		{
		}

		Memory::PhysAddr VirtToPhys(uintptr_t addr)
		{
		}
	}
}
