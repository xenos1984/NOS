// Heap.cpp - Kernel heap management

#include <cstddef>
#include <Memory.h>
#include <Chunker.h>
#include <Pager.h>
#include <Symbol.h>
#include <AtomicOps.h>
#include <Console.h>
#include <heap/Heap.h>

namespace Kernel
{
	namespace Heap
	{
		struct MemoryPointer
		{
			uintptr_t mem;
			uintptr_t length : (8 * sizeof(uintptr_t) - 1);
			uintptr_t free : 1;
			MemoryPointer* prev;
			MemoryPointer* next;
		} PACKED;

		static const uintptr_t heapaddr = Symbol::heapStart.Addr();
		static const uintptr_t tabaddr = Symbol::heapTab.Addr();
		static const size_t heaplen = Symbol::heapTab.Addr() - Symbol::heapStart.Addr();
		static const size_t tablen = Symbol::heapEnd.Addr() - Symbol::heapTab.Addr();

		static MemoryPointer root {heapaddr, heaplen, 1, &root, &root};
		static MemoryPointer* const table = static_cast<MemoryPointer*>(Symbol::heapTab.Ptr());
		static unsigned long count = 0;
		static const unsigned long cmax = tablen / sizeof(MemoryPointer) - 1;
		static const size_t bs = 1 << Memory::MinPageBits;
		static const unsigned int cpb = bs / sizeof(MemoryPointer);

		static AtomicLock lock;

		void AllocChunk(MemoryPointer* mp)
		{
			unsigned long lower, upper;

			if((mp->prev->free && RoundDown(mp->mem, bs) >= (unsigned long)mp->prev->mem) || mp == &root)
				lower = RoundDown(mp->mem, bs);
			else
				lower = RoundUp(mp->mem, bs);

			if((mp->next->free && RoundUp(mp->next->mem, bs) <= (unsigned long)mp->next->mem + mp->next->length) || mp->next == &root)
				upper = RoundUp(mp->next->mem, bs);
			else
				upper = RoundDown(mp->next->mem, bs);

			if(upper > lower)
				AllocBlocks(lower, (upper - lower) / bs);
		}

		void FreeChunk(MemoryPointer* mp)
		{
			unsigned long lower, upper;

			if(mp != &root && mp->prev->free && RoundDown(mp->mem, bs) >= (unsigned long)mp->prev->mem)
				lower = RoundDown(mp->mem, bs);
			else
				lower = RoundUp(mp->mem, bs);

			if(mp->next != &root && mp->next->free && RoundUp(mp->next->mem, bs) <= (unsigned long)mp->next->mem + mp->next->length)
				upper = RoundUp(mp->next->mem, bs);
			else
				upper = RoundDown(mp->next->mem, bs);

			if(upper > lower)
				FreeBlocks(lower, (upper - lower) / bs);
		}

		void Merge(MemoryPointer* mp)
		{
			MemoryPointer* mpn = mp->next;

			mp->length += mpn->length;
			mpn->next->prev = mp;
			mp->next = mpn->next;
			count--;
			if(mpn != &table[count])
			{
				mpn->mem = table[count].mem;
				mpn->length = table[count].length;
				mpn->free = table[count].free;
				mpn->prev = table[count].prev;
				mpn->next = table[count].next;
				mpn->prev->next = mpn;
				mpn->next->prev = mpn;
			}
			if(count % cpb == 0)
				FreeBlock(tabaddr + (count / cpb) * bs);
		}

		void Split(MemoryPointer* mp, size_t n)
		{
			if(count % cpb == 0)
				AllocBlock(tabaddr + (count / cpb) * bs);
			table[count].mem = mp->mem + n;
			table[count].length = mp->length - n;
			table[count].free = mp->free;
			table[count].prev = mp;
			table[count].next = mp->next;
			mp->length = n;
			mp->next->prev = &table[count];
			mp->next = &table[count];
			count++;
		}

		void Free(void* addr, bool physalloc)
		{
			MemoryPointer* mp = &root;

			lock.Enter();
			do
			{
				if(mp->mem == (uintptr_t)addr)
				{
					if(physalloc)
						FreeChunk(mp);
					mp->free = 1;
					if(mp->next->free && mp->next != &root)
						Merge(mp);
					if(mp->prev->free && mp != &root)
						Merge(mp->prev);
					lock.Exit();
					return;
				}
				mp = mp->next;
			}
			while(mp != &root);
			lock.Exit();
		}

		void* Alloc(size_t bytes, bool physalloc)
		{
			size_t n = RoundUp(bytes, sizeof(int));
			MemoryPointer* mp = &root;

			if(count == cmax)
				return(nullptr);

			lock.Enter();
			do
			{
				if(mp->free && mp->length >= n)
				{
					if(mp->length > n)
						Split(mp, n);
					mp->free = 0;
					if(physalloc)
						AllocChunk(mp);
					lock.Exit();
					return((void*)(mp->mem));
				}
				mp = mp->next;
			}
			while(mp != &root);
			lock.Exit();

			return(nullptr);
		}

		void* Alloc(size_t bytes, size_t align, bool physalloc)
		{
			size_t n = RoundUp(bytes, sizeof(int));
			MemoryPointer* mp = &root;

			if(count == cmax)
				return(nullptr);

			lock.Enter();
			do
			{
				if(mp->free && mp->length >= RoundDiff(mp->mem, align) + n)
				{
					if(mp->length - RoundDiff(mp->mem, align) > n)
						Split(mp, n + RoundDiff(mp->mem, align));
					if(mp->length > n)
					{
						Split(mp, RoundDiff(mp->mem, align));
						mp = mp->next;
					}
					mp->free = 0;
					if(physalloc)
						AllocChunk(mp);
					lock.Exit();
					return((void*)(mp->mem));
				}
				mp = mp->next;
			}
			while(mp != &root);
			lock.Exit();

			return(nullptr);
		}

		void ShowMap(void)
		{
			MemoryPointer* mp = &root;

			Console::WriteFormat("count = 0x%8x, cpb = 0x%8x, size = 0x%8x, total = 0x%8x\n", count, cpb, sizeof(MemoryPointer), cmax);
			Console::WriteLine("Heap map:");
			lock.Enter();
			do
			{
				Console::WriteFormat("Block 0x%8x of length 0x%8x at 0x%8x, %s\n", mp, mp->length, mp->mem, (mp->free ? "free" : "allocated"));
				mp = mp->next;
			}
			while(mp != &root);
			lock.Exit();
		}
	}
}

