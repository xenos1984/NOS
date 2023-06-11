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
/*
		template<Memory::PageBits size> uintptr_t TryAllocBlock(uintptr_t virt, uintptr_t length, typename std::enable_if<!Chunker::IsValidSize(size), Memory::MemType>::type type, Memory::Zone zone);
		template<Memory::PageBits size> uintptr_t TryAllocBlock(uintptr_t virt, uintptr_t length, typename std::enable_if<Chunker::IsValidSize(size), Memory::MemType>::type type, Memory::Zone zone);

		template<Memory::PageBits size> uintptr_t TryAllocBlock(uintptr_t virt, uintptr_t length, typename std::enable_if<!Chunker::IsValidSize(size), Memory::MemType>::type type, Memory::Zone zone)
		{
			return TryAllocBlock<(Memory::PageBits)(size - 1)>(virt, length, type, zone);
		}

		template<Memory::PageBits size> uintptr_t TryAllocBlock(uintptr_t virt, uintptr_t length, typename std::enable_if<Chunker::IsValidSize(size), Memory::MemType>::type type, Memory::Zone zone)
		{
			if(((virt & ((1 << size) - 1)) == 0) && (length >= (1 << size)))
			{
				if(AllocBlock<size>(virt, type, zone))
					return (1 << size);

				return 0;
			}
			else
				return TryAllocBlock<(Memory::PageBits)(size - 1)>(virt, length, type, zone);
		}

		template<> uintptr_t TryAllocBlock<MinPageBits>(uintptr_t virt, [[maybe_unused]] uintptr_t length, Memory::MemType type, Memory::Zone zone)
		{
			AllocBlock<MinPageBits>(virt, type, zone);
			return MinPageSize;
		}

		void* Alloc(uintptr_t virt, size_t length, Memory::MemType type, Memory::Zone zone)
		{
			uintptr_t diff, alloc;

//			Console::WriteFormat("Alloc 0x%8x bytes with type %u in zone %u at 0x%8x.\n", length, type, zone, virt);

			// Check alignment of supplied memory locations.
			if(virt & MinPageMask)
				return nullptr;

			if(length & MinPageMask)
				return nullptr;

			// Now that everything is checked, we can actually start allocating.
			for(diff = 0; diff < length; )
			{
				alloc = TryAllocBlock<MaxPageBits>(virt + diff, length - diff, type, zone);

				if(!alloc)
				{
					if(diff)
						Free(virt, diff);

					return nullptr;
				}

				diff += alloc;
			}

			return (void*)virt;
		}

		template<Memory::PageBits size> void TryFreeBlock(uintptr_t addr, typename std::enable_if<!Chunker::IsValidSize(size), Memory::PageBits>::type mapped);
		template<Memory::PageBits size> void TryFreeBlock(uintptr_t addr, typename std::enable_if<Chunker::IsValidSize(size), Memory::PageBits>::type mapped);

		template<Memory::PageBits size> void TryFreeBlock(uintptr_t addr, typename std::enable_if<!Chunker::IsValidSize(size), Memory::PageBits>::type mapped)
		{
			TryFreeBlock<(Memory::PageBits)(size - 1)>(addr, mapped);
		}

		template<Memory::PageBits size> void TryFreeBlock(uintptr_t addr, typename std::enable_if<Chunker::IsValidSize(size), Memory::PageBits>::type mapped)
		{
			if(size == mapped)
				FreeBlock<size>(addr);
			else
				TryFreeBlock<(Memory::PageBits)(size - 1)>(addr, mapped);
		}

		template<> void TryFreeBlock<Pager::MinPageBits>(uintptr_t addr, [[maybe_unused]] Memory::PageBits mapped)
		{
			FreeBlock<MinPageBits>(addr);
		}

		bool Free(uintptr_t virt, size_t length)
		{
			uintptr_t addr;
			uintptr_t end = virt + length;

			for(addr = virt; addr < end; )
			{
				//Memory::PageBits mapped = MappedSize(addr);
				//TryFreeBlock<MaxPageBits>(addr, mapped);
				//addr += (1ULL << mapped);
			}

			return true;
		}
*/	}
}
