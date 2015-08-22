// Chunker.cpp - Physical memory manager aka "chunker" function implementations.

#include <limits>
#include <new>
#include <chunker/bitmap/Chunker.h>
#include <Memory.h>
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

			void Free(Memory::PhysAddr start, Memory::PhysAddr length)
			{
			}

			void Reserve(Memory::PhysAddr addr)
			{
				unsigned long n = ((addr - start) >> Memory::MinPageBits) / std::numeric_limits<unsigned long>::digits;
				unsigned long b = ((addr - start) >> Memory::MinPageBits) % std::numeric_limits<unsigned long>::digits;

				bitmap[n] |= 1UL << b;
			}

			void Reserve(Memory::PhysAddr start, Memory::PhysAddr length)
			{
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

			// Mark all memory as allocated / reserved and let kernel free unused memory.
			for(i = 0; i < fbmlen; i++)
				firstbitmap[i] = ~0UL;

			Console::WriteMessage(Console::Style::INFO, "Chunker:", "Started with %d MB starting at %d MB in zone %d.", length >> 20, start >> 20, zone);
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

			if(r != nullptr)
			{
				r->Free(addr);
				return true;
			}
			else
				return false;
		}

		bool Free(Memory::PhysAddr start, Memory::PhysAddr length)
		{
			Region* r = FindRegion(start);

			if(r != nullptr)
			{
				r->Free(start, length);
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

		bool Reserve(Memory::PhysAddr start, Memory::PhysAddr length)
		{
			Region* r = FindRegion(start);

			if(r != nullptr)
			{
				r->Reserve(start, length);
				return true;
			}
			else
				return false;
		}
	}
}
