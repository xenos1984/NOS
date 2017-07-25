// Pager.cpp - Working with page tables.

#include <cstdint>
#include <new>
#include <Pager.h>
#include <Symbol.h>

namespace Kernel
{
	namespace Pager
	{
		static uintptr_t VirtMapEnd = Symbol::kernelEnd.Addr();

		void* MapBootRegion(Memory::PhysAddr start, Memory::PhysAddr length, Memory::MemType type)
		{
			Memory::PhysAddr first, last;
			uintptr_t virt;

			first = start & ~Memory::PGM_4K;
			last = ((start + length - 1) & ~Memory::PGM_4K) + Memory::PGS_4K;

			if(!Map(first, VirtMapEnd, last - first, type))
				return nullptr;

			virt = VirtMapEnd + (start & Memory::PGM_4K);
			VirtMapEnd += (last - first);
			return reinterpret_cast<void*>(virt);
		}

		void* MapBootString(Memory::PhysAddr start, Memory::MemType type)
		{
			Memory::PhysAddr first;
			void* ret;
			char* p;

			first = start & ~Memory::PGM_4K;

			if(!Map(first, VirtMapEnd, Memory::PGS_4K, type))
				return nullptr;

			ret = reinterpret_cast<void*>(VirtMapEnd + (start & Memory::PGM_4K));
			p = reinterpret_cast<char*>(ret);

			while(true)
			{
				VirtMapEnd += Memory::PGS_4K;
				first += Memory::PGS_4K;

				do
				{
					if(!(*(p++)))
						return(ret);
				}
				while((Memory::PGM_4K & reinterpret_cast<uintptr_t>(p)) != 0);

				if(!Map(first, VirtMapEnd, Memory::PGS_4K, type))
					return nullptr;
			}
		}

		void FreeBootMemory(void)
		{
		}
	}
}

