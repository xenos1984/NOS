// VirtualMemory.cpp - Virtual memory management

#include <cstddef>
#include <new>
#include <VirtualMemory.h>
#include <Console.h>
#include <Symbol.h>

#define ROUND_UP(ADDR,ALIGN) ((((unsigned long)(ADDR) - 1) & (-(ALIGN))) + (ALIGN))
#define ROUND_DOWN(ADDR,ALIGN) ((unsigned long)(ADDR) & (-(ALIGN)))
#define ROUND_DIFF(ADDR,ALIGN) ((ALIGN) - 1 - (((unsigned long)(ADDR) - 1) & ((ALIGN) - 1)))

using namespace Kernel;

char virtmem_space[sizeof(VirtualMemory)];

SECTION(".init.text") VirtualMemory::VirtualMemory(void)
{
	void* tabaddr;
	void* heapaddr;
	unsigned long tablen, heaplen;

	blocksize = physmem().GetBlockSize();
	countperblock = blocksize / sizeof(MemoryPointer);
	heapaddr = Symbol::heapStart.Ptr();
	tabaddr = Symbol::heapTab.Ptr();
	heaplen = Symbol::heapTab.Addr() - Symbol::heapStart.Addr();
	tablen = Symbol::heapEnd.Addr() - Symbol::heapTab.Addr();
	physmem().AllocBlocks(tabaddr, 1);
	base = (MemoryPointer*)tabaddr;
	counttotal = tablen / sizeof(MemoryPointer);
	count = 1;
	base->mem = heapaddr;
	base->length = heaplen;
	base->free = 1;
	base->prev = base->next = base;
}

void* VirtualMemory::Alloc(unsigned long bytes, bool physalloc)
{
	unsigned long n = ROUND_UP(bytes,sizeof(int));
	MemoryPointer* mp = base;

	if(count == counttotal)
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
			return(mp->mem);
		}
		mp = mp->next;
	}
	while(mp != base);
	lock.Exit();

	return(nullptr);
}

void* VirtualMemory::Alloc(unsigned long bytes, unsigned long align, bool physalloc)
{
	unsigned long n = ROUND_UP(bytes,sizeof(int));
	MemoryPointer* mp = base;

	if(count == counttotal)
		return(nullptr);

	lock.Enter();
	do
	{
		if(mp->free && mp->length >= ROUND_DIFF(mp->mem,align) + n)
		{
			if(mp->length - ROUND_DIFF(mp->mem,align) > n)
				Split(mp, n + ROUND_DIFF(mp->mem,align));
			if(mp->length > n)
			{
				Split(mp, ROUND_DIFF(mp->mem,align));
				mp = mp->next;
			}
			mp->free = 0;
			if(physalloc)
				AllocChunk(mp);
			lock.Exit();
			return(mp->mem);
		}
		mp = mp->next;
	}
	while(mp != base);
	lock.Exit();

	return(nullptr);
}

void VirtualMemory::Free(void* addr, bool physalloc)
{
	MemoryPointer* mp = base;

	lock.Enter();
	do
	{
		if(mp->mem == addr)
		{
			if(physalloc)
				FreeChunk(mp);
			mp->free = 1;
			if(mp->next->free && mp->next != base)
				Merge(mp);
			if(mp->prev->free && mp != base)
				Merge(mp->prev);
			lock.Exit();
			return;
		}
		mp = mp->next;
	}
	while(mp != base);
	lock.Exit();
}

void VirtualMemory::Split(MemoryPointer* mp, unsigned long n)
{
	if(count % countperblock == 0)
		physmem().AllocBlocks((void*)((unsigned long)base + (count / countperblock) * blocksize), 1);
	base[count].mem = (void*)(n + (unsigned long)mp->mem);
	base[count].length = mp->length - n;
	base[count].free = mp->free;
	base[count].prev = mp;
	base[count].next = mp->next;
	mp->length = n;
	mp->next->prev = &base[count];
	mp->next = &base[count];
	count++;
}

void VirtualMemory::Merge(MemoryPointer* mp)
{
	MemoryPointer* mpn = mp->next;

	mp->length += mpn->length;
	mpn->next->prev = mp;
	mp->next = mpn->next;
	count--;
	if(mpn != &base[count])
	{
		mpn->mem = base[count].mem;
		mpn->length = base[count].length;
		mpn->free = base[count].free;
		mpn->prev = base[count].prev;
		mpn->next = base[count].next;
		mpn->prev->next = mpn;
		mpn->next->prev = mpn;
	}
	if(count % countperblock == 0)
		physmem().FreeBlocks((void*)((unsigned long)base + (count / countperblock) * blocksize), 1);
}

void VirtualMemory::FreeChunk(MemoryPointer* mp)
{
	unsigned long lower, upper;

	if(mp != base && mp->prev->free && ROUND_DOWN(mp->mem,blocksize) >= (unsigned long)mp->prev->mem)
		lower = ROUND_DOWN(mp->mem,blocksize);
	else
		lower = ROUND_UP(mp->mem,blocksize);

	if(mp->next != base && mp->next->free && ROUND_UP(mp->next->mem,blocksize) <= (unsigned long)mp->next->mem + mp->next->length)
		upper = ROUND_UP(mp->next->mem,blocksize);
	else
		upper = ROUND_DOWN(mp->next->mem,blocksize);

	if(upper > lower)
		physmem().FreeBlocks((void*)lower, (upper - lower) / blocksize);
}

void VirtualMemory::AllocChunk(MemoryPointer* mp)
{
	unsigned long lower, upper;

	if((mp->prev->free && ROUND_DOWN(mp->mem,blocksize) >= (unsigned long)mp->prev->mem) || mp == base)
		lower = ROUND_DOWN(mp->mem,blocksize);
	else
		lower = ROUND_UP(mp->mem,blocksize);

	if((mp->next->free && ROUND_UP(mp->next->mem,blocksize) <= (unsigned long)mp->next->mem + mp->next->length) || mp->next == base)
		upper = ROUND_UP(mp->next->mem,blocksize);
	else
		upper = ROUND_DOWN(mp->next->mem,blocksize);

	if(upper > lower)
		physmem().AllocBlocks((void*)lower, (upper - lower) / blocksize);
}

void VirtualMemory::ShowMap(void)
{
	MemoryPointer* mp = base;

	console().WriteFormat("count = 0x%8x, cpb = 0x%8x, size = 0x%8x, total = 0x%8x\n", count, countperblock, sizeof(MemoryPointer), counttotal);
	console().WriteLine("Heap map:");
	lock.Enter();
	do
	{
		console().WriteFormat("Block 0x%8x of length 0x%8x at 0x%8x, %s\n", mp, mp->length, mp->mem, (mp->free ? "free" : "allocated"));
		mp = mp->next;
	}
	while(mp != base);
	lock.Exit();
}

void* operator new(size_t count)
{
	return virtmem().Alloc(count);
}

void* operator new[](size_t count)
{
	return virtmem().Alloc(count);
}

void operator delete(void* pointer)
{
	virtmem().Free(pointer);
}

void operator delete[](void* pointer)
{
	virtmem().Free(pointer);
}

void operator delete(void* pointer, size_t size)
{
	virtmem().Free(pointer);
}

void operator delete[](void* pointer, size_t size)
{
	virtmem().Free(pointer);
}
