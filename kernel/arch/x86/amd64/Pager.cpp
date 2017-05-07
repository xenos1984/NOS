// Pager.cpp - Working with page tables.

#include <cstdint>
#include <new>
#include <Pager.h>
#include <Memory.h>
#include <Chunker.h>
#include <Console.h>
//#include INC_ARCH(PageTableEntry.h)
//#include INC_ARCH(PageTable.h)
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
