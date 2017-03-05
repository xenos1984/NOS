// Heap.h - Kernel heap management

#ifndef __HEAP_HEAP_H__
#define __HEAP_HEAP_H__

#include <Heap.h>
#include <Chunker.h>
#include <Pager.h>
#include <cstdint>

namespace Kernel
{
	namespace Heap
	{
		inline uintptr_t RoundUp(uintptr_t addr, uintptr_t align = sizeof(long))
		{
			return ((addr - 1) & (-align)) + align;
		}

		inline uintptr_t RoundDown(uintptr_t addr, uintptr_t align = sizeof(long))
		{
			return addr & (-align);
		}

		inline uintptr_t RoundDiff(uintptr_t addr, uintptr_t align = sizeof(long))
		{
			return align - 1 - ((addr - 1) & (align - 1));
		}

		void AllocBlock(uintptr_t addr)
		{
			Memory::PhysAddr phys = Chunker::Alloc(static_cast<Memory::Zone>(0));
			Pager::MapPage<Memory::MinPageBits>(phys, addr, Memory::MemType::KERNEL_RW);
		}

		void FreeBlock(uintptr_t addr)
		{
			Memory::PhysAddr phys = Pager::VirtToPhys(addr);
			Pager::UnmapPage<Memory::MinPageBits>(addr);
			Chunker::Free(phys);
		}

		void AllocBlocks(uintptr_t addr, unsigned int count)
		{
			for(unsigned int i = 0; i < count; i++)
			{
				AllocBlock(addr);
				addr += (1 << Memory::MinPageBits);
			}
		}

		void FreeBlocks(uintptr_t addr, unsigned int count)
		{
			for(unsigned int i = 0; i < count; i++)
			{
				FreeBlock(addr);
				addr += (1 << Memory::MinPageBits);
			}
		}
	}
}

#endif

