// Pager.cpp - Working with page tables.

#include <cstdint>
#include <new>
#include <Pager.h>
#include <Symbol.h>
#include <Console.h>
#include INC_SUBARCH(PageTable.h)
#include INC_ARCH(Coprocessor.h)

namespace Kernel
{
	namespace Pager
	{
		inline void InvalidateAll(void)
		{
			Coprocessor::TLBIALL::Write(0);
		}

		inline void InvalidateASID(uint8_t asid)
		{
			Coprocessor::TLBIASID::Write(asid);
		}

		inline void InvalidateMVAAll(uintptr_t mva)
		{
			Coprocessor::TLBIMVAA::Write(mva);
		}

		inline void InvalidateMVA(uint8_t asid, uintptr_t mva)
		{
			Coprocessor::TLBIMVA::Write(mva | asid);
		}

		template<Memory::PageBits bits> void MapPage(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type)
		{
			static_assert(IsValidSize(bits), "invalid page size");
		}

		template<Memory::PageBits bits> void UnmapPage(uintptr_t virt)
		{
			static_assert(IsValidSize(bits), "invalid page size");
		}

		template<> void MapPage<Memory::PGB_4K>(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type)
		{
		}

		template<> void UnmapPage<Memory::PGB_4K>(uintptr_t virt)
		{
		}

		template<> void MapPage<Memory::PGB_64K>(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type)
		{
		}

		template<> void UnmapPage<Memory::PGB_64K>(uintptr_t virt)
		{
		}

		template<> void MapPage<Memory::PGB_1M>(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type)
		{
		}

		template<> void UnmapPage<Memory::PGB_1M>(uintptr_t virt)
		{
		}

		template<> void MapPage<Memory::PGB_16M>(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type)
		{
		}

		template<> void UnmapPage<Memory::PGB_16M>(uintptr_t virt)
		{
		}

		Memory::PhysAddr VirtToPhys(uintptr_t addr)
		{
			unsigned int tab = addr >> Memory::PGB_1M;
			unsigned int entry = (addr >> Memory::PGB_4K) & 0xff;

			PageTableEntryL1& pgl1 = KernelPTL1().Entry(tab);

			Console::WriteFormat("Addr 0x%8x -> PT1 Entry 0x%3x : 0x%8x\n", addr, tab, pgl1);

			if(pgl1.IsFault())
				return ~0;

			if(pgl1.IsSuper())
				return pgl1.Phys() | (addr & Memory::PGM_16M);

			if(pgl1.IsSection())
				return pgl1.Phys() | (addr & Memory::PGM_1M);

			PageTableEntryL2& pgl2 = PageTableL2::Table(tab).Entry(entry);

			Console::WriteFormat("Addr 0x%8x -> PT2 0x%3x Entry 0x%2x : 0x%8x\n", addr, tab, entry, pgl2);

			if(pgl2.IsLarge())
				return pgl2.Phys() | (addr & Memory::PGM_64K);

			if(pgl2.IsSmall())
				return pgl2.Phys() | (addr & Memory::PGM_4K);

			return ~0;
		}
	}
}

