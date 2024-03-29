// Chunker.cpp - Physical memory manager aka "chunker" function implementations.

//#include <limits>
#include <atomic>
#include <new>
#include <Chunker.h>
#include <Memory.h>
#include <Symbol.h>
#include <Console.h>

namespace Kernel
{
	namespace Chunker
	{
		static const unsigned long bitwidth = 8 * sizeof(unsigned long); //std::numeric_limits<unsigned long>::digits;

		struct Region
		{
			Region* prev;
			Region* next;
			const Memory::PhysAddr start;
			const Memory::PhysAddr length;
			std::atomic_ulong free;
			const unsigned long total;
			std::atomic_ulong* const bitmap;

			constexpr Region(void)
				: prev(nullptr)
				, next(nullptr)
				, start(0)
				, length(0)
				, free(0)
				, total(0)
				, bitmap(nullptr)
			{};

			Region(Memory::PhysAddr s, Memory::PhysAddr l, std::atomic_ulong* b = nullptr)
				: prev(nullptr)
				, next(nullptr)
				, start(s)
				, length(l)
				, free(length >> MinPageBits)
				, total(((free - 1) & (-bitwidth)) + bitwidth)
				, bitmap(b == nullptr ? new std::atomic_ulong[total / bitwidth] : b)
			{
				if(free < total)
					bitmap[total / bitwidth - 1] = ~((1UL << (free % bitwidth)) - 1);
			}

			bool Contains(Memory::PhysAddr addr) const
			{
				return(addr >= start && addr - start < length);
			}

			bool IsAlloc(Memory::PhysAddr addr)
			{
				unsigned long n = ((addr - start) >> MinPageBits) / bitwidth;
				unsigned long b = ((addr - start) >> MinPageBits) % bitwidth;

				return bitmap[n] & (1UL << b);
			}

			void Free(Memory::PhysAddr addr)
			{
				unsigned long n = ((addr - start) >> MinPageBits) / bitwidth;
				unsigned long b = ((addr - start) >> MinPageBits) % bitwidth;

				if(bitmap[n].fetch_and(~(1UL << b)) & (1UL << b))
					free++;
			}

			void Free(Memory::PhysAddr first, Memory::PhysAddr last)
			{
				for(Memory::PhysAddr addr = first; addr < last; addr += (1UL << MinPageBits))
					Free(addr);
			}

			void Reserve(Memory::PhysAddr addr)
			{
				unsigned long n = ((addr - start) >> MinPageBits) / bitwidth;
				unsigned long b = ((addr - start) >> MinPageBits) % bitwidth;

				if(!(bitmap[n].fetch_or(1UL << b) & (1UL << b)))
					free--;
			}

			void Reserve(Memory::PhysAddr first, Memory::PhysAddr last)
			{
				for(Memory::PhysAddr addr = first; addr < last; addr += (1UL << MinPageBits))
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
								//Console::WriteFormat("Alloc memory: base = 0x%p, total = 0x%x, n = 0x%x, b = 0x%x, free = %d\n", start, total, n, b, free.load());
								return start + ((n * bitwidth + b) << MinPageBits);
							}
						}
					}
				}

				return 0;
			}
		};

		static const unsigned int fbmlen = Memory::MaxInitPages / bitwidth;
		static std::atomic_ulong firstbitmap[fbmlen];

		static Region firstregion;
		static Region* regions[static_cast<int>(Memory::Zone::MAX) + 1];

		void SECTION(".init.text") Init(Memory::PhysAddr start, Memory::PhysAddr length, Memory::Zone zone)
		{
			unsigned int i;

			// Initial state: no memory in any zone.
			for(i = 0; i <= static_cast<int>(Memory::Zone::MAX); i++)
				regions[i] = nullptr;

			// Initial memory belongs to this zone.
			regions[static_cast<int>(zone)] = &firstregion;

			// If start is not at page boundary, round up.
			length -= MinPageMask - ((start - 1) & MinPageMask);
			start = MinPageSize + ((start - 1) & ~MinPageMask);

			// If end is not at page boundary, round down.
			length = length & ~MinPageMask;

			// Enter first zone properties.
			new (&firstregion) Region(start, length, firstbitmap);
			firstregion.prev = firstregion.next = &firstregion;

			// Mark all memory as free and let kernel mark used memory before the first allocation.
			for(i = 0; i < fbmlen; i++)
				firstbitmap[i] = 0UL;

			Console::WriteMessage(Console::Style::OK, "Chunker:", "%u kB from %p to %p in zone %u.", length >> 10, start, start + length - 1, zone);
		}

		void AddRegion(Memory::PhysAddr start, Memory::PhysAddr length, Memory::Zone zone)
		{
			// If start is not at page boundary, round up.
			length -= MinPageMask - ((start - 1) & MinPageMask);
			start = MinPageSize + ((start - 1) & ~MinPageMask);

			// If end is not at page boundary, round down.
			length = length & ~MinPageMask;

			Region* rz = regions[static_cast<int>(zone)];

			// Check regions for overlap and add only new parts.
			if(rz != nullptr)
			{
				do
				{
					if(start >= rz->start + rz->length)
						continue;

					if(rz->start >= start + length)
						continue;

					if(start < rz->start)
					{
						if(start + length > rz->start + rz->length) // Old region is contained in new region: need to add two separate regions.
						{
							AddRegion(start, rz->start - length, zone);
							AddRegion(rz->start + rz->length, start + length - rz->length, zone);
						}
						else // Old region covers upper part of new region.
							length = rz->start - start;
					}
					else
					{
						if(start + length > rz->start + rz->length) // Old region covers lower part of new region.
						{
							length -= rz->start + rz->length - start;
							start = rz->start + rz->length;
						}
						else // New region is contained in old region.
							return;
					}

					rz = rz->next;
				}
				while(rz != regions[static_cast<int>(zone)]);
			}

			Region* r = new Region(start, length);

			if(rz == nullptr)
			{
				regions[static_cast<int>(zone)] = r->prev = r->next = r;
			}
			else
			{
				r->next = rz;
				r->prev = rz->prev;
				rz->prev->next = r;
				rz->prev = r;
			}

			Console::WriteMessage(Console::Style::INFO, "Chunker:", "Added memory from %p to %p in zone %u.", start, start + length - 1, zone);
		}

		template<Memory::PageBits bits> Memory::PhysAddr Alloc(Memory::Zone zone __attribute__((unused)))
		{
			static_assert(IsValidSize(bits), "invalid chunk size");
			return 0;
		}

		template<Memory::PageBits bits> bool Free(Memory::PhysAddr phys __attribute__((unused)))
		{
			static_assert(IsValidSize(bits), "invalid chunk size");
			return false;
		}

		template<Memory::PageBits bits> bool Reserve(Memory::PhysAddr phys __attribute__((unused)))
		{
			static_assert(IsValidSize(bits), "invalid chunk size");
			return false;
		}

		template<> Memory::PhysAddr Alloc<MinPageBits>(Memory::Zone zone)
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
					r = r->prev;
					if(r->Contains(addr))
						return r;
				}
				while(r != regions[i]);
			}

			return nullptr;
		}

		State IsAlloc(Memory::PhysAddr addr)
		{
			Region* r = FindRegion(addr);

			if(r == nullptr)
				return State::Invalid;

			return r->IsAlloc(addr) ? State::Alloc : State::Free;
		}

		template<> bool Free<MinPageBits>(Memory::PhysAddr addr)
		{
			Region* r = FindRegion(addr);

			// Console::WriteFormat("Free memory at 0x%p\n", addr);
			if(r == nullptr)
				return false;

			r->Free(addr);
			return true;
		}

		bool Free(Memory::PhysAddr first, Memory::PhysAddr last)
		{
			Region* r = FindRegion(first);

			// Console::WriteFormat("Free memory from 0x%p to 0x%p\n", first, last);
			if(r == nullptr)
				return false;

			r->Free(first, last);
			return true;
		}

		template<> bool Reserve<MinPageBits>(Memory::PhysAddr addr)
		{
			Region* r = FindRegion(addr);

			if(r == nullptr)
				return false;

			r->Reserve(addr);
			return true;
		}

		bool Reserve(Memory::PhysAddr first, Memory::PhysAddr last)
		{
			Region* r = FindRegion(first);

			//Console::WriteMessage(Console::Style::INFO, "Chunker:", "Reserving memory from %p to %p.", first, last - 1);

			if(r == nullptr)
				return false;

			r->Reserve(first, last);
			return true;
		}
	}
}
