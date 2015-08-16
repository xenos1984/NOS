// Chunker.cpp - Physical memory manager aka "chunker" function implementations.

#include <limits>
#include <new>
#include <chunker/bitmap/Chunker.h>
#include <Memory.h>
#include <AtomicOps.h>

using namespace Kernel;

namespace Kernel
{
	namespace Chunker
	{
		struct Region
		{
			Region* prev;
			Region* next;
			Memory::PhysAddr start;
			Memory::PhysAddr length;
			unsigned long* bitmap;
			Memory::Zone zone;

			constexpr Region(Memory::PhysAddr s, Memory::PhysAddr l, unsigned long* b, Memory::Zone z, Region* p = nullptr, Region* n = nullptr) : prev(p), next(n), start(s), length(l), bitmap(b), zone(z) {};
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
			firstregion.start = start;
			firstregion.length = length;
			firstregion.zone = zone;

			// Mark all memory as allocated / reserved and let kernel free unused memory.
			for(i = 0; i < fbmlen; i++)
				firstbitmap[i] = ~0UL;
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
	}
}
