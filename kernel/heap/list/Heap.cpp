// Heap.cpp - Kernel heap management

#include <cstddef>
#include <Allocator.h>
#include <Symbol.h>
#include <SpinLock.h>
#include <Console.h>
#include <heap/Heap.h>
#include <new>

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

			constexpr MemoryPointer(uintptr_t m, uintptr_t l, uintptr_t f, MemoryPointer* p, MemoryPointer* n) : mem(m), length(l), free(f), prev(p), next(n) {};
			MemoryPointer(const MemoryPointer&) = delete;

			MemoryPointer(MemoryPointer&& old)
			{
				mem = old.mem;
				length = old.length;
				free = old.free;
				prev = old.prev;
				next = old.next;
				prev->next = this;
				next->prev = this;
			}
		} PACKED;

		static MemoryPointer root = MemoryPointer(0, 0, 1, &root, &root);
		static unsigned long count = 0;
		static constexpr size_t bs = 1 << Allocator::MinPageBits;
		static constexpr unsigned int cpb = bs / sizeof(MemoryPointer);

		static SpinLock lock;

		inline MemoryPointer& MemPtr(unsigned long index)
		{
			return static_cast<MemoryPointer*>(Symbol::heapTab.Ptr())[index];
		}

		inline uintptr_t MemAddr(unsigned long index)
		{
			return Symbol::heapTab.Addr() + index * sizeof(MemoryPointer);
		}

		inline unsigned long CountMax(void)
		{
			return (Symbol::heapEnd.Addr() - Symbol::heapTab.Addr()) / sizeof(MemoryPointer) - 1;
		}

		void Init(void)
		{
			new (&root) MemoryPointer(Symbol::heapStart.Addr(), Symbol::heapTab.Addr() - Symbol::heapStart.Addr(), 1, &root, &root);
			Console::WriteMessage(Console::Style::INFO, "Heap:", "Started with %d kB starting at %p.", root.length >> 10, root.mem);
		}

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
				Allocator::AllocBlocks<Allocator::MinPageBits>(lower, (upper - lower) / bs);
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
				Allocator::FreeBlocks<Allocator::MinPageBits>(lower, (upper - lower) / bs);
		}

		void Merge(MemoryPointer* mp)
		{
			MemoryPointer* mpn = mp->next;

			mp->length += mpn->length;
			mpn->next->prev = mp;
			mp->next = mpn->next;
			count--;
			MemoryPointer& last = MemPtr(count);
			if(mpn != &last)
				new (mpn) MemoryPointer(std::move(last));
/*			{
				mpn->mem = table[count].mem;
				mpn->length = table[count].length;
				mpn->free = table[count].free;
				mpn->prev = table[count].prev;
				mpn->next = table[count].next;
				mpn->prev->next = mpn;
				mpn->next->prev = mpn;
			}
*/			if(count % cpb == 0)
			Allocator::FreeBlock<Allocator::MinPageBits>(MemAddr(count));
		}

		void Split(MemoryPointer* mp, size_t n)
		{
			if(count % cpb == 0)
				Allocator::AllocBlock<Allocator::MinPageBits>(MemAddr(count), Memory::MemType::KERNEL_RW);
			MemoryPointer& last = MemPtr(count);
			new (&last) MemoryPointer(mp->mem + n, mp->length - n, mp->free, mp, mp->next);
			mp->length = n;
			mp->next->prev = &last;
			mp->next = &last;
			count++;
		}

		void Free(void* addr, bool physalloc)
		{
			MemoryPointer* mp = &root;

			lock.Lock();
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
					lock.Unlock();
					return;
				}
				mp = mp->next;
			}
			while(mp != &root);
			lock.Unlock();
		}

		void* Alloc(size_t bytes, bool physalloc)
		{
			size_t n = RoundUp(bytes, sizeof(int));
			MemoryPointer* mp = &root;

			if(count == CountMax())
				return(nullptr);

			lock.Lock();
			do
			{
				if(mp->free && mp->length >= n)
				{
					if(mp->length > n)
						Split(mp, n);
					mp->free = 0;
					if(physalloc)
						AllocChunk(mp);
					lock.Unlock();
					return((void*)(mp->mem));
				}
				mp = mp->next;
			}
			while(mp != &root);
			lock.Unlock();

			return(nullptr);
		}

		void* Alloc(size_t bytes, size_t align, bool physalloc)
		{
			size_t n = RoundUp(bytes, sizeof(int));
			MemoryPointer* mp = &root;

			if(count == CountMax())
				return(nullptr);

			lock.Lock();
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
					lock.Unlock();
					return((void*)(mp->mem));
				}
				mp = mp->next;
			}
			while(mp != &root);
			lock.Unlock();

			return(nullptr);
		}

		void ShowMap(void)
		{
			MemoryPointer* mp = &root;

			Console::WriteFormat("count = 0x%8x, cpb = 0x%8x, size = 0x%8x, total = 0x%8x\n", count, cpb, sizeof(MemoryPointer), CountMax());
			Console::WriteLine("Heap map:");
			lock.Lock();
			do
			{
				Console::WriteFormat("Block 0x%p of length 0x%p at 0x%p, %s\n", mp, mp->length, mp->mem, (mp->free ? "free" : "allocated"));
				mp = mp->next;
			}
			while(mp != &root);
			lock.Unlock();
		}
	}
}
