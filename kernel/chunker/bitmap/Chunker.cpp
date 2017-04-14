// Chunker.cpp - Physical memory manager aka "chunker" function implementations.

#include <limits>
#include <new>
#include <chunker/bitmap/Chunker.h>
#include <Memory.h>
#include <Symbol.h>
#include <AtomicOps.h>
#include <Console.h>

namespace Kernel
{
	namespace Chunker
	{
		struct Region
		{
			Region* prev;
			Region* next;
			const Memory::PhysAddr start;
			const Memory::PhysAddr length;
			unsigned long* const bitmap;
			const Memory::Zone zone;

			constexpr Region(Memory::PhysAddr s, Memory::PhysAddr l, unsigned long* b, Memory::Zone z, Region* p = nullptr, Region* n = nullptr) : prev(p), next(n), start(s), length(l), bitmap(b), zone(z) {};

			bool Contains(Memory::PhysAddr addr) const
			{
				return(addr >= start && addr - start < length);
			}

			void Free(Memory::PhysAddr addr)
			{
				unsigned long n = ((addr - start) >> Memory::MinPageBits) / std::numeric_limits<unsigned long>::digits;
				unsigned long b = ((addr - start) >> Memory::MinPageBits) % std::numeric_limits<unsigned long>::digits;

				bitmap[n] &= ~(1UL << b);
			}

			void Free(Memory::PhysAddr first, Memory::PhysAddr last)
			{
				for(Memory::PhysAddr addr = first; addr < last; addr += (1UL << Memory::MinPageBits))
					Free(addr);
			}

			void Reserve(Memory::PhysAddr addr)
			{
				unsigned long n = ((addr - start) >> Memory::MinPageBits) / std::numeric_limits<unsigned long>::digits;
				unsigned long b = ((addr - start) >> Memory::MinPageBits) % std::numeric_limits<unsigned long>::digits;

				bitmap[n] |= 1UL << b;
			}

			void Reserve(Memory::PhysAddr first, Memory::PhysAddr last)
			{
				for(Memory::PhysAddr addr = first; addr < last; addr += (1UL << Memory::MinPageBits))
					Reserve(addr);
			}

			Memory::PhysAddr Alloc(void)
			{
				unsigned long n = (length >> Memory::MinPageBits) / std::numeric_limits<unsigned long>::digits;
				unsigned long b = std::numeric_limits<unsigned long>::digits;

				while(n > 0)
				{
					n--;
					if(bitmap[n] != ~0UL)
					{
						while(b > 0)
						{
							b--;
							if(!(bitmap[n] & (1UL << b)))
							{
								bitmap[n] |= 1UL << b;
								// Console::WriteFormat("Alloc memory: base = 0x%p, n = 0x%x, b = 0x%x\n", start, n, b);
								return start + ((n * std::numeric_limits<unsigned long>::digits + b) << Memory::MinPageBits);
							}
						}
					}
				}

				return 0;
			}
		};

		static const unsigned int fbmlen = Memory::MaxInitPages / std::numeric_limits<unsigned long>::digits;
		static unsigned long firstbitmap[fbmlen];

		static Region firstregion = Region(0, 0, firstbitmap, static_cast<Memory::Zone>(0), &firstregion, &firstregion);
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
			Region* r = new Region(start, length, new unsigned long[(length >> Memory::MinPageBits) / std::numeric_limits<unsigned long>::digits], zone);
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

		Memory::PhysAddr Alloc(Memory::Zone zone)
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
