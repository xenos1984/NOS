// Pager.cpp - Working with page tables.

#include <cstdint>
#include <Pager.h>
#include <Memory.h>
#include INC_ARCH(ControlRegisters.h)

namespace Kernel
{
	namespace Pager
	{
		static const unsigned int PAGES_PER_TABLE = 1024;
		static const unsigned int PAGE_RECURSIVE = 1023;
		static const uintptr_t REC_PAGE_DIR = (((uintptr_t)PAGE_RECURSIVE) << 22) + (((uintptr_t)PAGE_RECURSIVE) << 12);
		static const uintptr_t REC_PAGE_TAB = ((uintptr_t)PAGE_RECURSIVE) << 22;

		union PageTableEntry
		{
			struct
			{
				Memory::PhysAddr present : 1;
				Memory::PhysAddr writeable : 1;
				Memory::PhysAddr user : 1;
				Memory::PhysAddr writethrough : 1;
				Memory::PhysAddr cachedisable : 1;
				Memory::PhysAddr accessed : 1;
				Memory::PhysAddr dirty : 1;
				Memory::PhysAddr large : 1;
				Memory::PhysAddr global : 1;
				Memory::PhysAddr : 3;
				Memory::PhysAddr page: std::numeric_limits<Memory::PhysAddr>::digits - 12;
			} bits PACKED;
			Memory::PhysAddr raw;
		};

		struct PageTable
		{
			PageTableEntry page[PAGES_PER_TABLE];
		} PACKED;

		class PageDirectory
		{
		private:
			PageTableEntry page[PAGES_PER_TABLE * PAGES_PER_TABLE];

		public:
			template<Memory::PageBits bits> bool MapPage(Memory::PhysAddr phys, uintptr_t virt, unsigned int flags)
			{
				static_assert(IsValidSize(bits), "illegal page size");
			}

			template<Memory::PageBits bits> bool UnmapPage(uintptr_t virt)
			{
				static_assert(IsValidSize(bits), "illegal page size");
			}
		};

		template<> bool PageDirectory::MapPage<Memory::PAGE_4K>(Memory::PhysAddr phys, uintptr_t virt, unsigned int flags)
		{
		}

		template<> bool PageDirectory::MapPage<Memory::PAGE_4M>(Memory::PhysAddr phys, uintptr_t virt, unsigned int flags)
		{
			if(phys & 0x3fffff)
				return false;
			if(virt & 0x3fffff)
				return false;

			PageTableEntry& pte = page[PAGE_RECURSIVE * PAGES_PER_TABLE + (virt >> 22)];

			if(pte.bits.present)
				return false;

			pte.raw = phys;
			pte.bits.large = 1;
			pte.bits.global = 1;
			pte.bits.present = 1;
		}

		template<> bool PageDirectory::UnmapPage<Memory::PAGE_4K>(uintptr_t virt)
		{
		}

		template<> bool PageDirectory::UnmapPage<Memory::PAGE_4M>(uintptr_t virt)
		{
		}

		static PageDirectory& RecPageDir = *(PageDirectory*)REC_PAGE_TAB;

		void Test(void)
		{
			RecPageDir.MapPage<Memory::PAGE_4K>(0x100000, 0, 0);
			RecPageDir.MapPage<Memory::PAGE_4M>(0x100000, 0, 0);
		}
	}
}
