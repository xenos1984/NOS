// Allocator.h - Physical to virtual memory management, allocation and deallocation.

#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

#include <Memory.h>
#include <Chunker.h>
#include <Pager.h>

namespace Kernel
{

	namespace Allocator
	{
		template<Memory::PageBits bits> void* AllocBlock(uintptr_t virt, Memory::MemType type = Memory::MemType::KERNEL_RW, Memory::Zone zone = static_cast<Memory::Zone>(0));
		template<Memory::PageBits bits> inline void* AllocBlock(void* virt, Memory::MemType type = Memory::MemType::KERNEL_RW, Memory::Zone zone = static_cast<Memory::Zone>(0)) { return AllocBlock<bits>((uintptr_t)virt, type, zone); }
		template<Memory::PageBits bits> bool FreeBlock(uintptr_t virt);
		template<Memory::PageBits bits> inline bool FreeBlock(void* virt) { return FreeBlock<bits>((uintptr_t)virt); }

		template<Memory::PageBits bits> void* AllocBlocks(uintptr_t virt, unsigned int n, Memory::MemType type = Memory::MemType::KERNEL_RW, Memory::Zone zone = static_cast<Memory::Zone>(0));
		template<Memory::PageBits bits> inline void* AllocBlocks(void* virt, unsigned int n, Memory::MemType type = Memory::MemType::KERNEL_RW, Memory::Zone zone = static_cast<Memory::Zone>(0)) { return AllocBlocks<bits>((uintptr_t)virt, n, type, zone); }
		template<Memory::PageBits bits> bool FreeBlocks(uintptr_t virt, unsigned int n);
		template<Memory::PageBits bits> inline bool FreeBlocks(void* virt, unsigned int n) { return FreeBlocks<bits>((uintptr_t)virt, n); }
/*
		void* Alloc(uintptr_t virt, uintptr_t length, Memory::MemType type = Memory::MemType::KERNEL_RW, Memory::Zone zone = static_cast<Memory::Zone>(0));
		inline void* Alloc(void* virt, uintptr_t length, Memory::MemType type = Memory::MemType::KERNEL_RW, Memory::Zone zone = static_cast<Memory::Zone>(0)) { return Alloc((uintptr_t)virt, length, type, zone); }
		bool Free(uintptr_t virt, uintptr_t length);
		inline bool Free(void* virt, uintptr_t length) { return Free((uintptr_t)virt, length); }
*/
		static const Memory::PageBits MinPageBits = (Chunker::MinPageBits < Pager::MinPageBits ? Chunker::MinPageBits : Pager::MinPageBits);
		static const Memory::PageBits MaxPageBits = (Chunker::MaxPageBits > Pager::MaxPageBits ? Chunker::MaxPageBits : Pager::MaxPageBits);

		static const Memory::PageSize MinPageSize = static_cast<Memory::PageSize>(1ULL << MinPageBits);
		static const Memory::PageSize MaxPageSize = static_cast<Memory::PageSize>(1ULL << MaxPageBits);

		static const Memory::PageMask MinPageMask = static_cast<Memory::PageMask>(MinPageSize - 1);
		static const Memory::PageMask MaxPageMask = static_cast<Memory::PageMask>(MaxPageSize - 1);
	}
}
#endif
