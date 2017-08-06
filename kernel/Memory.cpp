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

		template<PageBits bits> void* AllocBlocks(uintptr_t virt, unsigned int n, MemType type, Zone zone)
		{
			for(uintptr_t addr = virt; addr < virt + (((uintptr_t)n) << bits); addr += 1ULL << bits)
			{
				if(AllocBlock<bits>(addr, type, zone) == nullptr)
				{
					while(addr > virt)
					{
						addr -= 1ULL << bits;
						FreeBlock<bits>(addr);
					}

					return nullptr;
				}
			}
			return (void*)virt;
		}

		template<PageBits bits> bool FreeBlocks(uintptr_t virt, unsigned int n)
		{
			bool ok = true;
			for(uintptr_t addr = virt; addr < virt + (((uintptr_t)n) << bits); addr += 1ULL << bits)
				ok = FreeBlock<bits>(addr) && ok;
			return ok;
		}

		template void* AllocBlock<MinPageBits>(uintptr_t virt, MemType type, Zone zone);
		template bool FreeBlock<MinPageBits>(uintptr_t virt);

		template void* AllocBlocks<MinPageBits>(uintptr_t virt, unsigned int n, MemType type, Zone zone);
		template bool FreeBlocks<MinPageBits>(uintptr_t virt, unsigned int n);
	}
}

