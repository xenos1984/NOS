// Memory.cpp - General memory information and management.

#include <Memory.h>
#include <Chunker.h>
#include <Pager.h>

namespace Kernel
{
	namespace Memory
	{
		template<PageBits bits> void* AllocBlock(uintptr_t virt, MemType type, Zone zone)
		{
			PhysAddr phys = Chunker::Alloc<bits>(zone);
			if(!phys)
				return nullptr;

			Pager::MapPage<bits>(phys, virt, type);
			return (void*)virt;
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

		template void* AllocBlock<MinPageBits>(uintptr_t virt, MemType type, Zone zone);
		template bool FreeBlock<MinPageBits>(uintptr_t virt);
	}
}

