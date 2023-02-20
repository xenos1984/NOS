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

		extern "C" void* memset(void* bufptr, int value, size_t size)
		{
			unsigned char* buf = (unsigned char*)bufptr;

			for (size_t i = 0; i < size; i++)
				buf[i] = (unsigned char)value;

			return bufptr;
		}

		extern "C" void* memmove(void* dstptr, const void* srcptr, size_t size)
		{
			unsigned char* dst = (unsigned char*)dstptr;
			const unsigned char* src = (const unsigned char*)srcptr;

			if (dst < src)
			{
				for (size_t i = 0; i < size; i++)
					dst[i] = src[i];
			}
			else
			{
				for (size_t i = size; i != 0; i--)
					dst[i - 1] = src[i - 1];
			}

			return dstptr;
		}

		extern "C" void* memcpy(void* __restrict dstptr, const void* __restrict srcptr, size_t size)
		{
			unsigned char* dst = (unsigned char*)dstptr;
			const unsigned char* src = (const unsigned char*)srcptr;

			for (size_t i = 0; i < size; i++)
				dst[i] = src[i];

			return dstptr;
		}

		extern "C" int memcmp(const void* aptr, const void* bptr, size_t size)
		{
			const unsigned char* a = (const unsigned char*) aptr;
			const unsigned char* b = (const unsigned char*) bptr;

			for (size_t i = 0; i < size; i++)
			{
				if (a[i] < b[i])
					return -1;
				else if (b[i] < a[i])
					return 1;
			}

			return 0;
		}
	}
}
