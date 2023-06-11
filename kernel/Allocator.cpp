// Allocator.cpp - Physical to virtual memory management, allocation and deallocation.

#include <Allocator.h>

namespace Kernel
{
	namespace Allocator
	{
		template<Memory::PageBits bits> void* AllocBlock(uintptr_t virt, Memory::MemType type, Memory::Zone zone)
		{
			Memory::PhysAddr phys = Chunker::Alloc<bits>(zone);
			if(!phys)
				return nullptr;

			Pager::MapPage<bits>(phys, virt, type);
			return (void*)virt;
		}

		template<Memory::PageBits bits> bool FreeBlock(uintptr_t virt)
		{
			Memory::PhysAddr phys = Pager::VirtToPhys(virt);
			if(!phys)
				return false;

			Pager::UnmapPage<bits>(virt);
			Chunker::Free<bits>(phys);
			return true;
		}

		template<Memory::PageBits bits> void* AllocBlocks(uintptr_t virt, unsigned int n, Memory::MemType type, Memory::Zone zone)
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

		template<Memory::PageBits bits> bool FreeBlocks(uintptr_t virt, unsigned int n)
		{
			bool ok = true;
			for(uintptr_t addr = virt; addr < virt + (((uintptr_t)n) << bits); addr += 1ULL << bits)
				ok = FreeBlock<bits>(addr) && ok;
			return ok;
		}

		template void* AllocBlock<MinPageBits>(uintptr_t virt, Memory::MemType type, Memory::Zone zone);
		template bool FreeBlock<MinPageBits>(uintptr_t virt);

		template void* AllocBlocks<MinPageBits>(uintptr_t virt, unsigned int n, Memory::MemType type, Memory::Zone zone);
		template bool FreeBlocks<MinPageBits>(uintptr_t virt, unsigned int n);
	}
}
