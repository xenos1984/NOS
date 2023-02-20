// Pager.cpp - Working with page tables.

#include <cstdint>
#include <new>
#include <Pager.h>
#include <Symbol.h>
#include INC_ARCH(PageTable.h)
#include INC_ARCH(ControlRegisters.h)

namespace Kernel
{
	namespace Pager
	{
		static uintptr_t VirtMapEnd = Symbol::kernelEnd.Addr();

		inline void Invalidate(uintptr_t virt)
		{
			asm volatile ("invlpg (%0)" : : "r"(virt) : "memory");
		}

		/** Determine which attributes to set for the page table mapping this page. */
		static constexpr Memory::MemType ParentType(Memory::MemType type)
		{
			switch(type)
			{
			case Memory::MemType::KERNEL_EXEC:
			case Memory::MemType::KERNEL_RO:
			case Memory::MemType::KERNEL_RW:
			case Memory::MemType::KERNEL_PAGETAB:
				return Memory::MemType::KERNEL_PAGETAB;
				break;
			case Memory::MemType::USER_EXEC:
			case Memory::MemType::USER_RO:
			case Memory::MemType::USER_RW:
			case Memory::MemType::USER_PAGETAB:
			case Memory::MemType::USER_COW:
			case Memory::MemType::USER_DEMAND:
				return Memory::MemType::USER_PAGETAB;
				break;
			default:
				return Memory::MemType::KERNEL_RW;
			}
		}

		template<unsigned int level> PageTableLevel<level>& PageTableLevel<level>::Create(unsigned long i, Memory::MemType type)
		{
			static_assert(level > 0, "Top level page table cannot be created.");
			static_assert(level < PAGE_LEVELS, "Table level exceeds number of paging levels.");

			uintptr_t virt = PAGE_TABLE_ADDR[level + 1] + i * sizeof(PageTableLevel<level>);
			Memory::AllocBlock<Memory::PGB_4K>(virt, type);
			new (reinterpret_cast<PageTableLevel<level>*>(virt)) PageTableLevel<level>;

			return *reinterpret_cast<PageTableLevel<level>*>(virt);
		}

		template<unsigned int level> void PageTableLevel<level>::Destroy(void)
		{
			static_assert(level > 0, "Top level page table cannot be destroyed.");
			static_assert(level < PAGE_LEVELS - 1, "Table level exceeds number of paging levels.");

			for(int i = 0; i < (1 << PAGE_BITS[level]); i++)
			{
				if(this->entry[i].IsPresent())
				{
					if(this->entry[i].IsLarge())
					{
						// TODO: Unmap pages that are larger than the chunker supports.
						//Chunker::Free<PageLevelSize<level>>(this->entry[i].Phys());
					}
					else
					{
						PageTableLevel<level + 1>::Table((Index() << PAGE_BITS[level]) + i).Destroy();
						Chunker::Free<Memory::PGB_4K>(this->entry[i].Phys());
					}
					this->entry[i].Clear();
				}
			}
		}

		template<> void PageTableLevel<PAGE_LEVELS - 1>::Destroy(void)
		{
			for(int i = 0; i < (1 << PAGE_BITS[PAGE_LEVELS - 1]); i++)
			{
				if(this->entry[i].IsPresent())
				{
					Chunker::Free<PageLevelSize<PAGE_LEVELS - 1>>(this->entry[i].Phys());
					this->entry[i].Clear();
				}
			}
		}

		template<> void PageTableLevel<0>::Destroy(void)
		{
			for(int i = 0; i < (1 << PAGE_BITS[0]); i++)
			{
				if(this->entry[i].IsPresent())
				{
					if(this->entry[i].IsLarge())
					{
						//Chunker::Free<PageLevelSize<0>>(this->entry[i].Phys());
					}
					else
					{
						PageTableLevel<1>::Table((Index() << PAGE_BITS[0]) + i).Destroy();
						Chunker::Free<Memory::PGB_4K>(this->entry[i].Phys());
					}
					this->entry[i].Clear();
				}
			}
		}

		template<Memory::PageBits bits> void MapPage(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type)
		{
			static_assert(IsValidSize(bits), "invalid page size");
			static_assert(PageSizeLevel<bits> < PAGE_LEVELS, "page table level too large");

			unsigned long tab = (virt & 0xffffffffffffULL) >> (bits + PAGE_BITS[PageSizeLevel<bits>]);
			unsigned long entry = (virt >> bits) & ((1 << PAGE_BITS[PageSizeLevel<bits>]) - 1);

			if(!PageTableLevel<PageSizeLevel<bits>>::Exists(tab))
				PageTableLevel<PageSizeLevel<bits>>::Create(tab, ParentType(type));

			PageTableEntry& pte = PageTableLevel<PageSizeLevel<bits>>::Table(tab).Entry(entry);
			pte.Set<bits>(phys, type);
			Invalidate(virt);
		}

		template<Memory::PageBits bits> void UnmapPage(uintptr_t virt)
		{
			static_assert(IsValidSize(bits), "invalid page size");
			static_assert(PageSizeLevel<bits> < PAGE_LEVELS, "page table level too large");

			unsigned int tab = (virt & 0xffffffffffffULL) >> (bits + PAGE_BITS[PageSizeLevel<bits>]);
			unsigned long entry = (virt >> bits) & ((1 << PAGE_BITS[PageSizeLevel<bits>]) - 1);

			PageTableLevel<PageSizeLevel<bits>>& pt = PageTableLevel<PageSizeLevel<bits>>::Table(tab);
			PageTableEntry& pte = pt.Entry(entry);
			pte.Clear();
			Invalidate(virt);

			if(pt.IsEmpty())
				Memory::FreeBlock<Memory::PGB_4K>(&pt);
		}

#if (defined ARCH_X86_IA32) && (!defined CONFIG_PAE)
		template<> void MapPage<Memory::PGB_4M>(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type)
		{
			unsigned long entry = virt >> Memory::PGB_4M;

			PageTableEntry& pte = PageTableTop().Entry(entry);
			pte.Set<Memory::PGB_4M>(phys, type);
			Invalidate(virt);
		}

		template<> void UnmapPage<Memory::PGB_4M>(uintptr_t virt)
		{
			unsigned long entry = virt >> Memory::PGB_4M;

			PageTableEntry& pte = PageTableTop().Entry(entry);
			pte.Clear();
			Invalidate(virt);
		}
#endif

		template void MapPage<Memory::PGB_4K>(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type);
		template void UnmapPage<Memory::PGB_4K>(uintptr_t virt);
#if (defined CONFIG_PAE) || (defined ARCH_X86_AMD64)
		template void MapPage<Memory::PGB_2M>(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type);
		template void UnmapPage<Memory::PGB_2M>(uintptr_t virt);
//#else
//		template void MapPage<Memory::PGB_4M>(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type);
//		template void UnmapPage<Memory::PGB_4M>(uintptr_t virt);
#endif
#ifdef ARCH_X86_AMD64
		template void MapPage<Memory::PGB_1G>(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type);
		template void UnmapPage<Memory::PGB_1G>(uintptr_t virt);
#endif

		template<> PageTableLevel<0>::PageTableLevel(void)
		{
			for(int i = 1 << (PAGE_BITS[0] - 1); i < (1 << PAGE_BITS[0]); i++)
				entry[i] = PageTableTop().Entry(i);
			entry[PAGE_RECURSIVE].Set<Memory::PGB_4K>(VirtToPhys(this), Memory::MemType::KERNEL_PAGETAB);
		}

		PageTableLevel<0>* CreateContext(void)
		{
			PageTableLevel<0>* pt = new PageTableLevel<0>;
			return pt;
		}

		Memory::PhysAddr SwitchContext(Memory::PhysAddr phys)
		{
			Memory::PhysAddr old;

			old = CR3::Read();
			CR3::Write((old & Memory::PGM_4K) | phys);

			return(old & ~Memory::PGM_4K);
		}

		void DeleteContext(PageTableLevel<0>* pt)
		{
			Memory::PhysAddr cr3;

			cr3 = SwitchContext(VirtToPhys(pt));
			PageTableTop().Destroy();
			SwitchContext(cr3);
			delete pt;
		}

		void* MapBootRegion(Memory::PhysAddr start, Memory::PhysAddr length, Memory::MemType type)
		{
			Memory::PhysAddr first, last;
			uintptr_t virt;

			first = start & ~Memory::PGM_4K;
			last = ((start + length - 1) & ~Memory::PGM_4K) + Memory::PGS_4K;

			if(!Map(first, VirtMapEnd, last - first, type))
				return nullptr;

			virt = VirtMapEnd + (start & Memory::PGM_4K);
			VirtMapEnd += (last - first);
			return reinterpret_cast<void*>(virt);
		}

		void* MapBootString(Memory::PhysAddr start, Memory::MemType type)
		{
			Memory::PhysAddr first;
			void* ret;
			char* p;

			first = start & ~Memory::PGM_4K;

			if(!Map(first, VirtMapEnd, Memory::PGS_4K, type))
				return nullptr;

			ret = reinterpret_cast<void*>(VirtMapEnd + (start & Memory::PGM_4K));
			p = reinterpret_cast<char*>(ret);

			while(true)
			{
				VirtMapEnd += Memory::PGS_4K;
				first += Memory::PGS_4K;

				do
				{
					if(!(*(p++)))
						return(ret);
				}
				while((Memory::PGM_4K & reinterpret_cast<uintptr_t>(p)) != 0);

				if(!Map(first, VirtMapEnd, Memory::PGS_4K, type))
					return nullptr;
			}
		}

		void FreeBootMemory(void)
		{
		}
	}
}
