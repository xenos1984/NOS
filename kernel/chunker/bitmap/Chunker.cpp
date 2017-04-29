// Chunker.cpp - Physical memory manager aka "chunker" function implementations.

#include <limits>
#include <atomic>
#include <new>
#include <chunker/bitmap/Chunker.h>
#include <Memory.h>
#include <Symbol.h>
#include <Console.h>

namespace Kernel
{
	namespace Chunker
	{
		struct Region
		{
			static const int bitwidth = std::numeric_limits<unsigned long>::digits;

			Region* prev;
			Region* next;
			const Memory::PhysAddr start;
			const Memory::PhysAddr end;
			std::atomic_ulong* const bitmap;
			const Memory::Zone zone;
			const unsigned long total;
			std::atomic_ulong free;

			constexpr Region(Memory::PhysAddr s, Memory::PhysAddr l, std::atomic_ulong* b, Memory::Zone z, Region* p = nullptr, Region* n = nullptr) : prev(p), next(n), start(s), end(s + l), bitmap(b), zone(z), total(l >> Memory::MinPageBits), free(l >> Memory::MinPageBits) {};

			bool Contains(Memory::PhysAddr addr) const
			{
				return(addr >= start && addr - start < end);
			}

			void Free(Memory::PhysAddr addr)
			{
				unsigned long n = ((addr - start) >> Memory::MinPageBits) / bitwidth;
				unsigned long b = ((addr - start) >> Memory::MinPageBits) % bitwidth;

				if(bitmap[n].fetch_and(~(1UL << b)) & (1UL << b))
					free++;
			}

			void Free(Memory::PhysAddr first, Memory::PhysAddr last)
			{
				for(Memory::PhysAddr addr = first; addr < last; addr += (1UL << Memory::MinPageBits))
					Free(addr);
			}

			void Reserve(Memory::PhysAddr addr)
			{
				unsigned long n = ((addr - start) >> Memory::MinPageBits) / bitwidth;
				unsigned long b = ((addr - start) >> Memory::MinPageBits) % bitwidth;

				if(!(bitmap[n].fetch_or(1UL << b) & (1UL << b)))
					free--;
			}

			void Reserve(Memory::PhysAddr first, Memory::PhysAddr last)
			{
				for(Memory::PhysAddr addr = first; addr < last; addr += (1UL << Memory::MinPageBits))
					Reserve(addr);
			}

			Memory::PhysAddr Alloc(void)
			{
				unsigned long n = total / bitwidth;
				unsigned long b = bitwidth;
				unsigned long x;

				if(!free)
					return 0;

				while(n > 0)
				{
					n--;
					if(bitmap[n] != ~0UL)
					{
						x = bitmap[n].exchange(~0UL);
						if(x == ~0UL)
							continue;

						while(b > 0)
						{
							b--;
							if(!(x & (1UL << b)))
							{
								x |= 1UL << b;
								bitmap[n] = x;
								free--;
								// Console::WriteFormat("Alloc memory: base = 0x%p, n = 0x%x, b = 0x%x, free = %d\n", start, n, b, free.load());
								return start + ((n * bitwidth + b) << Memory::MinPageBits);
							}
						}
					}
				}

				return 0;
			}
		};

		static const unsigned int fbmlen = Memory::MaxInitPages / std::numeric_limits<unsigned long>::digits;
		static std::atomic_ulong firstbitmap[fbmlen];

		static Region firstregion {0, 0, firstbitmap, static_cast<Memory::Zone>(0), &firstregion, &firstregion};
		static Region* regions[static_cast<int>(Memory::Zone::MAX) + 1];

		void Init(Memory::PhysAddr start, Memory::PhysAddr length, Memory::Zone zone)
		{
			unsigned int i;

			// Initial state: no memory in any zone.
			for(i = 0; i <= static_cast<int>(Memory::Zone::MAX); i++)
				regions[i] = nullptr;

			// Initial memory belongs to this zone.
			regions[static_cast<int>(zone)] = &firstregion;

			// Enter first zone properties.
			new (&firstregion) Region(start, length, firstbitmap, zone, &firstregion, &firstregion);

			// Mark all memory as free and let kernel mark used memory before the first allocation.
			for(i = 0; i < fbmlen; i++)
				firstbitmap[i] = 0UL;

			Console::WriteMessage(Console::Style::INFO, "Chunker:", "Started with %d kB (%d kB free) starting at %p in zone %d.", length >> 10, length >> 10, start, zone);
		}

		void AddRegion(Memory::PhysAddr start, Memory::PhysAddr length, Memory::Zone zone)
		{
			Region* r = new Region(start, length, new std::atomic_ulong[(length >> Memory::MinPageBits) / std::numeric_limits<unsigned long>::digits], zone);
			Region** rz = &regions[static_cast<int>(zone)];

			if(*rz == nullptr)
			{
				*rz = r->prev = r->next = r;
			}
			else
			{
				r->next = *rz;
				r->prev = (*rz)->prev;
				(*rz)->prev->next = r;
				(*rz)->prev = r;
			}
		}

		template<Memory::PageBits bits> Memory::PhysAddr Alloc(Memory::Zone zone __attribute__((unused)))
		{
			static_assert(IsValidSize(bits), "invalid chunk size");
			return 0;
		}

		template<> Memory::PhysAddr Alloc<Memory::MinPageBits>(Memory::Zone zone)
		{
			unsigned int i = static_cast<int>(zone);
			Region* r;
			Memory::PhysAddr addr;

			do
			{
				r = regions[i];
				if(r != nullptr)
				{
					do
					{
						r = r->prev;
						addr = r->Alloc();
						// Console::WriteFormat("Alloc memory: 0x%p\n", addr);
						if(addr)
							return addr;
					}
					while(r != regions[i]);
				}
				i++;
			}
			while(i < static_cast<int>(Memory::Zone::MAX));

			return 0;
		}

		Region* FindRegion(Memory::PhysAddr addr)
		{
			unsigned int i;
			Region* r;

			for(i = 0; i <= static_cast<int>(Memory::Zone::MAX); i++)
			{
				if(regions[i] == nullptr)
					continue;

				r = regions[i];
				do
				{
					if(r->Contains(addr))
						return r;
				}
				while(r != regions[i]);
			}

			return nullptr;
		}

		bool Free(Memory::PhysAddr addr)
		{
			Region* r = FindRegion(addr);

			// Console::WriteFormat("Free memory at 0x%p\n", addr);
			if(r != nullptr)
			{
				r->Free(addr);
				return true;
			}
			else
				return false;
		}

		bool Free(Memory::PhysAddr first, Memory::PhysAddr last)
		{
			Region* r = FindRegion(first);

			// Console::WriteFormat("Free memory from 0x%p to 0x%p\n", first, last);
			if(r != nullptr)
			{
				r->Free(first, last);
				return true;
			}
			else
				return false;
		}

		bool Reserve(Memory::PhysAddr addr)
		{
			Region* r = FindRegion(addr);

			if(r != nullptr)
			{
				r->Reserve(addr);
				return true;
			}
			else
				return false;
		}

		bool Reserve(Memory::PhysAddr first, Memory::PhysAddr last)
		{
			Region* r = FindRegion(first);

			if(r != nullptr)
			{
				r->Reserve(first, last);
				return true;
			}
			else
				return false;
		}
	}
}
