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

		template<Memory::PageBits size> typename std::enable_if<!Chunker::IsValidSize(size), uintptr_t>::type TryAllocBlock(uintptr_t virt, uintptr_t length, Memory::MemType type, Memory::Zone zone);
		template<Memory::PageBits size> typename std::enable_if<Chunker::IsValidSize(size), uintptr_t>::type TryAllocBlock(uintptr_t virt, uintptr_t length, Memory::MemType type, Memory::Zone zone);

		template<Memory::PageBits size> typename std::enable_if<!Chunker::IsValidSize(size), uintptr_t>::type TryAllocBlock(uintptr_t virt, uintptr_t length, Memory::MemType type, Memory::Zone zone)
		{
			return TryAllocBlock<(Memory::PageBits)(size - 1)>(virt, length, type, zone);
		}

		template<Memory::PageBits size> typename std::enable_if<Chunker::IsValidSize(size), uintptr_t>::type TryAllocBlock(uintptr_t virt, uintptr_t length, Memory::MemType type, Memory::Zone zone)
		{
			if(((virt & ((1ULL << size) - 1)) == 0) && (length >= (1ULL << size)))
			{
				if(AllocBlock<size>(virt, type, zone))
					return (1ULL << size);

				return 0;
			}
			else
				return TryAllocBlock<(Memory::PageBits)(size - 1)>(virt, length, type, zone);
		}

		template<> uintptr_t TryAllocBlock<MinPageBits>(uintptr_t virt, [[maybe_unused]] uintptr_t length, Memory::MemType type, Memory::Zone zone)
		{
			if(AllocBlock<MinPageBits>(virt, type, zone))
				return MinPageSize;

			return 0;
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

		template<Memory::PageBits size> typename std::enable_if<!Chunker::IsValidSize(size), uintptr_t>::type TryFreeBlock(uintptr_t addr, uintptr_t length);
		template<Memory::PageBits size> typename std::enable_if<Chunker::IsValidSize(size), uintptr_t>::type TryFreeBlock(uintptr_t addr, uintptr_t length);

		template<Memory::PageBits size> typename std::enable_if<!Chunker::IsValidSize(size), uintptr_t>::type TryFreeBlock(uintptr_t addr, uintptr_t length)
		{
			return TryFreeBlock<(Memory::PageBits)(size - 1)>(addr, length);
		}

		template<Memory::PageBits size> typename std::enable_if<Chunker::IsValidSize(size), uintptr_t>::type TryFreeBlock(uintptr_t addr, uintptr_t length)
		{
			if(((addr & ((1ULL << size) - 1)) == 0) && (length >= (1ULL << size)))
			{
				FreeBlock<size>(addr);
				return (1ULL << size);
			}
			else
				return TryFreeBlock<(Memory::PageBits)(size - 1)>(addr, length);
		}

		template<> uintptr_t TryFreeBlock<Pager::MinPageBits>(uintptr_t addr, [[maybe_unused]] uintptr_t length)
		{
			return FreeBlock<MinPageBits>(addr);
		}

		bool Free(uintptr_t virt, size_t length)
		{
			uintptr_t addr;
			uintptr_t end = virt + length;

			for(addr = virt; addr < end; )
				addr += TryFreeBlock<MaxPageBits>(addr, end - addr);

			return true;
		}
	}
}
