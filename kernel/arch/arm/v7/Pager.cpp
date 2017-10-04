// Pager.cpp - Working with page tables.

#include <cstdint>
#include <new>
#include <Pager.h>
#include <Symbol.h>
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
	}
}

