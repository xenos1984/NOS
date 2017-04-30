// Memory.cpp - General memory information and management.

#include <Memory.h>
#include <Chunker.h>
#include <Pager.h>

namespace Kernel
{
	namespace Memory
	{
		template<PageBits bits> bool AllocBlock(uintptr_t virt, MemType type)
		{
			PhysAddr phys = Chunker::Alloc<bits>();
			if(!phys)
				return false;

			Pager::MapPage<bits>(phys, virt, type);
			return true;
		}

		template<PageBits bits> bool FreeBlock(uintptr_t virt)
		{
			PhysAddr phys = Pager::VirtToPhys(virt);
			if(!phys)
				return false;

			Pager::UnmapPage<bits>(virt);
			Chunker::Free<bits>(phys);
			return true;
		}

		template bool AllocBlock<MinPageBits>(uintptr_t virt, MemType type);
		template bool FreeBlock<MinPageBits>(uintptr_t virt);
	}
}

