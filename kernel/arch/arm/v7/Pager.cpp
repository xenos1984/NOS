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
		inline void InvalidateAll(void)
		{
			Sysreg::TLBIALL::Write(0);
		}

		inline void InvalidateASID(uint8_t asid)
		{
			Sysreg::TLBIASID::Write(asid);
		}

		inline void InvalidateMVAAll(uintptr_t mva)
		{
			Sysreg::TLBIMVAA::Write(mva);
		}

		inline void InvalidateMVA(uint8_t asid, uintptr_t mva)
		{
			Sysreg::TLBIMVA::Write(mva | asid);
		}

		Memory::PageBits MappedSize(uintptr_t virt)
		{
			// Normalize address to smallest possible page size.
			virt &= ~Memory::PGM_4K;

			// Check top level page table first.
			PageTableEntryL1& pte1 = PageTableTop::Entry(virt >> Memory::PGB_1M);

			// If it is not present, then this page is not mapped.
			if(pte1.IsFault())
				return Memory::PGB_INV;

			// If it is present and 16MB, then this page is mapped.
			if(pte1.IsSuper())
				return Memory::PGB_16M;

			// If it is present and 1MB, then this page is mapped.
			if(pte1.IsSection())
				return Memory::PGB_1M;

			// For a smaller page, we need to check the next level.
			PageTableEntryL2& pte2 = PageTableL2::Table(virt >> Memory::PGB_1M).Entry((virt >> Memory::PGB_4K) & 0xff);

			// If it is present and large, this page is mapped.
			if(pte2.IsLarge())
				return Memory::PGB_64K;

			// If it is present and small, this page is mapped.
			if(pte2.IsSmall())
				return Memory::PGB_4K;

			// Otherwise, it is not mapped.
			return Memory::PGB_INV;
		}

		PageTableL2& PageTableL2::Create(unsigned long i, Memory::MemType type)
		{
			uintptr_t virt = (i < PageTablesUser ? UserPTL2Base : KernelPTL2Base - PageTablesUser * sizeof(PageTableL2)) + i * sizeof(PageTableL2);
			PageTableL2* pt = reinterpret_cast<PageTableL2*>(virt);

			if(VirtToPhys(virt) == ~0UL)
				Memory::AllocBlock<Memory::PGB_4K>(virt & ~Memory::PGM_4K, type);

			new (pt) PageTableL2;

			PageTableTop::Entry(i).Set<Memory::PGB_4K>(VirtToPhys(virt), type);

			return *pt;
		}

		bool PageTableL2::IsEmpty()
		{
			for(const PageTableEntryL2& e: entry)
			{
				if(!e.IsClear())
					return false;
			}

			return true;
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
			unsigned long table = virt >> Memory::PGB_1M;
			unsigned long entry = (virt >> Memory::PGB_4K) & 0xff;

			if(!PageTableL2::Exists(table))
				PageTableL2::Create(table, type);

			PageTableL2& pt = PageTableL2::Table(table);
			PageTableEntryL2& pte = pt.Entry(entry);
			pte.Set<Memory::PGB_4K>(phys, type);
		}

		template<> void UnmapPage<Memory::PGB_4K>(uintptr_t virt)
		{
			unsigned long table = virt >> Memory::PGB_1M;
			unsigned long entry = (virt >> Memory::PGB_4K) & 0xff;

			PageTableL2& pt = PageTableL2::Table(table);
			PageTableEntryL2& pte = pt.Entry(entry);
			pte.Clear();

			if(pt.IsEmpty())
			{
				PageTableTop::Entry(table).Clear();

				unsigned long t0 = table & ~0x3UL;

				if(
					PageTableTop::Entry(t0).IsClear() &&
					PageTableTop::Entry(t0 + 1).IsClear() &&
					PageTableTop::Entry(t0 + 2).IsClear() &&
					PageTableTop::Entry(t0 + 3).IsClear()
				)
				{
					uintptr_t virt = (t0 < PageTablesUser ? UserPTL2Base : KernelPTL2Base - PageTablesUser * sizeof(PageTableL2)) + t0 * sizeof(PageTableL2);
					Memory::FreeBlock<Memory::PGB_4K>(virt);
				}
			}
		}

		template<> void MapPage<Memory::PGB_64K>(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type)
		{
			unsigned long table = virt >> Memory::PGB_1M;
			unsigned long entry = (virt >> Memory::PGB_4K) & 0xf0;

			if(!PageTableL2::Exists(table))
				PageTableL2::Create(table, type);

			PageTableL2& pt = PageTableL2::Table(table);
			for(unsigned int i = 0; i < 16; i++)
			{
				PageTableEntryL2& pte = pt.Entry(entry + i);
				pte.Set<Memory::PGB_64K>(phys, type);
			}
		}

		template<> void UnmapPage<Memory::PGB_64K>(uintptr_t virt)
		{
			unsigned long table = virt >> Memory::PGB_1M;
			unsigned long entry = (virt >> Memory::PGB_4K) & 0xff;

			PageTableL2& pt = PageTableL2::Table(table);
			for(unsigned int i = 0; i < 16; i++)
			{
				PageTableEntryL2& pte = pt.Entry(entry + i);
				pte.Clear();
			}

			if(pt.IsEmpty())
			{
				PageTableTop::Entry(table).Clear();

				unsigned long t0 = table & ~0x3UL;

				if(
					PageTableTop::Entry(t0).IsClear() &&
					PageTableTop::Entry(t0 + 1).IsClear() &&
					PageTableTop::Entry(t0 + 2).IsClear() &&
					PageTableTop::Entry(t0 + 3).IsClear()
				)
				{
					uintptr_t virt = (t0 < PageTablesUser ? UserPTL2Base : KernelPTL2Base - PageTablesUser * sizeof(PageTableL2)) + t0 * sizeof(PageTableL2);
					Memory::FreeBlock<Memory::PGB_4K>(virt);
				}
			}
		}

		template<> void MapPage<Memory::PGB_1M>(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type)
		{
			PageTableEntryL1& pte = PageTableTop::Entry(virt >> Memory::PGB_1M);
			pte.Set<Memory::PGB_1M>(phys, type);
		}

		template<> void UnmapPage<Memory::PGB_1M>(uintptr_t virt)
		{
			PageTableEntryL1& pte = PageTableTop::Entry(virt >> Memory::PGB_1M);
			pte.Clear();
			InvalidateMVAAll(virt);
		}

		template<> void MapPage<Memory::PGB_16M>(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type)
		{
			for(unsigned int i = 0; i < 16; i++)
			{
				PageTableEntryL1& pte = PageTableTop::Entry(((virt & ~Memory::PGM_16M) >> Memory::PGB_1M) + i);
				pte.Set<Memory::PGB_16M>(phys, type);
			}
		}

		template<> void UnmapPage<Memory::PGB_16M>(uintptr_t virt)
		{
			for(unsigned int i = 0; i < 16; i++)
			{
				PageTableEntryL1& pte = PageTableTop::Entry(((virt & ~Memory::PGM_16M) >> Memory::PGB_1M) + i);
				pte.Clear();
			}
			InvalidateMVAAll(virt);
		}

		Memory::PhysAddr VirtToPhys(uintptr_t addr)
		{
			unsigned int tab = addr >> Memory::PGB_1M;
			unsigned int entry = (addr >> Memory::PGB_4K) & 0xff;

			PageTableEntryL1& pgl1 = PageTableTop::Entry(tab);

			// Console::WriteFormat("Addr 0x%8x -> PT1 Entry 0x%3x : 0x%8x\n", addr, tab, pgl1);

			if(pgl1.IsFault())
				return ~0;

			if(pgl1.IsSuper())
				return pgl1.Phys() | (addr & Memory::PGM_16M);

			if(pgl1.IsSection())
				return pgl1.Phys() | (addr & Memory::PGM_1M);

			// Console::WriteFormat("L2 table @ 0x%8x\n", &PageTableL2::Table(tab));
			// Console::WriteFormat("L2 table entry @ 0x%8x\n", &PageTableL2::Table(tab).Entry(entry));

			PageTableEntryL2& pgl2 = PageTableL2::Table(tab).Entry(entry);

			// Console::WriteFormat("Addr 0x%8x -> PT2 0x%3x Entry 0x%2x : 0x%8x\n", addr, tab, entry, pgl2);

			if(pgl2.IsLarge())
				return pgl2.Phys() | (addr & Memory::PGM_64K);

			if(pgl2.IsSmall())
				return pgl2.Phys() | (addr & Memory::PGM_4K);

			return ~0;
		}
	}
}
