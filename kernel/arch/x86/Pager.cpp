// Pager.cpp - Working with page tables.

#include <cstdint>
#include <new>
#include <Pager.h>
#include <Symbol.h>
#include INC_ARCH(PageTable.h)

namespace Kernel
{
	namespace Pager
	{
		static uintptr_t VirtMapEnd = Symbol::kernelEnd.Addr();

		/** Determine which attributes to set for the page table mapping this page. */
		Memory::MemType ParentType(Memory::MemType type)
		{
			switch(type)
			{
			case Memory::MemType::KERNEL_EXEC:
			case Memory::MemType::KERNEL_RO:
			case Memory::MemType::KERNEL_RW:
				return Memory::MemType::KERNEL_RW;
				break;
			case Memory::MemType::USER_EXEC:
			case Memory::MemType::USER_RO:
			case Memory::MemType::USER_RW:
			case Memory::MemType::USER_COW:
			case Memory::MemType::USER_DEMAND:
				return Memory::MemType::USER_RW;
				break;
			default:
				return Memory::MemType::KERNEL_RW;
			}
		}

		template<Memory::PageBits bits> constexpr unsigned int PageSizeLevel = ~0;

#ifdef ARCH_X86_IA32
#ifdef CONFIG_PAE
		template<> constexpr unsigned int PageSizeLevel<Memory::PGB_4K> = 2;
		template<> constexpr unsigned int PageSizeLevel<Memory::PGB_2M> = 1;
#else
		template<> constexpr unsigned int PageSizeLevel<Memory::PGB_4K> = 1;
		template<> constexpr unsigned int PageSizeLevel<Memory::PGB_4M> = 0;
#endif
#endif
#ifdef ARCH_X86_AMD64
		template<> constexpr unsigned int PageSizeLevel<Memory::PGB_4K> = 3;
		template<> constexpr unsigned int PageSizeLevel<Memory::PGB_2M> = 2;
		template<> constexpr unsigned int PageSizeLevel<Memory::PGB_1G> = 1;
#endif

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

			unsigned int tab = (virt & 0xffffffffffffUL) >> (bits + PAGE_BITS[PageSizeLevel<bits>]);
			unsigned long entry = (virt >> bits) & ((1 << PAGE_BITS[PageSizeLevel<bits>]) - 1);

			PageTableLevel<PageSizeLevel<bits>>& pt = PageTableLevel<PageSizeLevel<bits>>::Table(tab);
			PageTableEntry& pte = pt.Entry(entry);
			pte.Clear();
			Invalidate(virt);

			if(pt.IsEmpty())
				pt.Destroy();
		}

		template void MapPage<Memory::PGB_4K>(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type);
		template void UnmapPage<Memory::PGB_4K>(uintptr_t virt);
#if (defined ARCH_X86_IA32) && (!defined CONFIG_PAE)
		template void MapPage<Memory::PGB_4M>(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type);
		template void UnmapPage<Memory::PGB_4M>(uintptr_t virt);
#else
		template void MapPage<Memory::PGB_2M>(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type);
		template void UnmapPage<Memory::PGB_2M>(uintptr_t virt);
#ifdef ARCH_X86_AMD64
		template void MapPage<Memory::PGB_1G>(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type);
		template void UnmapPage<Memory::PGB_1G>(uintptr_t virt);
#endif
#endif
/*
		template<unsigned int level, Memory::PageBits ts, Memory::PageBits es> static inline void MapPageAt(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type)
		{
			unsigned long tab = (virt & 0xffffffffffffULL) >> ts;
			unsigned long entry = (virt >> es) & ((1 << (ts - es)) - 1);

			if(!PageTableLevel<level>::Exists(tab))
				PageTableLevel<level>::Create(tab, ParentType(type));

			PageTableEntry& pte = PageTableLevel<level>::Table(tab).Entry(entry);
			pte.Set<es>(phys, type);
			Invalidate(virt);
		}

		template<unsigned int level, Memory::PageBits ts, Memory::PageBits es> static inline void UnmapPageAt(uintptr_t virt)
		{
			unsigned int tab = (virt & 0xffffffffffffUL) >> ts;
			unsigned long entry = (virt >> es) & ((1 << (ts - es)) - 1);

			PageTableLevel<level>& pt = PageTableLevel<level>::Table(tab);
			PageTableEntry& pte = pt.Entry(entry);
			pte.Clear();
			Invalidate(virt);

			if(pt.IsEmpty())
				pt.Destroy();
		}

		template<Memory::PageBits bits> void MapPage(Memory::PhysAddr phys __attribute__((unused)), uintptr_t virt __attribute__((unused)), Memory::MemType type __attribute__((unused)))
		{
			static_assert(IsValidSize(bits), "invalid page size");
		}

		template<Memory::PageBits bits> void UnmapPage(uintptr_t virt __attribute__((unused)))
		{
			static_assert(IsValidSize(bits), "invalid page size");
		}

#ifdef ARCH_X86_IA32
#ifdef CONFIG_PAE
		template<> void MapPage<Memory::PGB_4K>(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type)
		{
			MapPageAt<2, Memory::PGB_2M, Memory::PGB_4K>(phys, virt, type);
		}

		template<> void UnmapPage<Memory::PGB_4K>(uintptr_t virt)
		{
			UnmapPageAt<2, Memory::PGB_2M, Memory::PGB_4K>(virt);
		}

		template<> void MapPage<Memory::PGB_2M>(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type)
		{
			MapPageAt<1, Memory::PGB_1G, Memory::PGB_2M>(phys, virt, type);
		}

		template<> void UnmapPage<Memory::PGB_2M>(uintptr_t virt)
		{
			UnmapPageAt<1, Memory::PGB_1G, Memory::PGB_2M>(virt);
		}
#else
		template<> void MapPage<Memory::PGB_4K>(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type)
		{
			MapPageAt<1, Memory::PGB_4M, Memory::PGB_4K>(phys, virt, type);
		}

		template<> void UnmapPage<Memory::PGB_4K>(uintptr_t virt)
		{
			UnmapPageAt<1, Memory::PGB_4M, Memory::PGB_4K>(virt);
		}

		template<> void MapPage<Memory::PGB_4M>(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type)
		{
			MapPageAt<0, Memory::PGB_4G, Memory::PGB_4M>(phys, virt, type);
		}

		template<> void UnmapPage<Memory::PGB_4M>(uintptr_t virt)
		{
			UnmapPageAt<0, Memory::PGB_4G, Memory::PGB_4M>(virt);
		}
#endif
#endif
#ifdef ARCH_X86_AMD64
		template<> void MapPage<Memory::PGB_4K>(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type)
		{
			MapPageAt<3, Memory::PGB_2M, Memory::PGB_4K>(phys, virt, type);
		}

		template<> void UnmapPage<Memory::PGB_4K>(uintptr_t virt)
		{
			UnmapPageAt<3, Memory::PGB_2M, Memory::PGB_4K>(virt);
		}

		template<> void MapPage<Memory::PGB_2M>(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type)
		{
			MapPageAt<2, Memory::PGB_1G, Memory::PGB_2M>(phys, virt, type);
		}

		template<> void UnmapPage<Memory::PGB_2M>(uintptr_t virt)
		{
			UnmapPageAt<2, Memory::PGB_1G, Memory::PGB_2M>(virt);
		}

		template<> void MapPage<Memory::PGB_1G>(Memory::PhysAddr phys, uintptr_t virt, Memory::MemType type)
		{
			MapPageAt<1, Memory::PGB_512G, Memory::PGB_1G>(phys, virt, type);
		}

		template<> void UnmapPage<Memory::PGB_1G>(uintptr_t virt)
		{
			UnmapPageAt<1, Memory::PGB_512G, Memory::PGB_1G>(virt);
		}
#endif
*/
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

