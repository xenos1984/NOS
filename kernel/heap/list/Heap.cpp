// Heap.cpp - Kernel heap management

#include <cstddef>
#include <Memory.h>
#include <Chunker.h>
#include <Pager.h>
#include <Symbol.h>
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
		};

		static const uintptr_t heapaddr = Symbol::heapStart.Addr();
		static const uintptr_t tabaddr = Symbol::heapTab.Addr();
		static const size_t heaplen = Symbol::heapTab.Addr() - Symbol::heapStart.Addr();
		static const size_t tablen = Symbol::heapEnd.Addr() - Symbol::heapTab.Addr();

		static MemoryPointer root {heapaddr, heaplen, 1, &root, &root};
/*
		void Init(void* start, size_t length)
		{
			Memory::PhysAddr phys;

			phys = Chunker::Alloc();
			Pager::MapPage(phys, start, 0);
		}
*/
	}
}

