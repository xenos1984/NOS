// Heap.cpp - Kernel heap management

#include <cstddef>
#include <Memory.h>
#include <Chunker.h>
#include <Pager.h>
#include <heap/Heap.h>

namespace Kernel
{
	namespace Heap
	{
		struct Chunk
		{
			size_t length : (8 * sizeof(size_t) - 1);
			size_t free : 1;
			Chunk* prev;
			Chunk* next;
			unsigned char data[0];
		};

		void Init(void* start, size_t length)
		{
			Memory::PhysAddr phys;

			phys = Chunker::Alloc();
			Pager::MapPage(phys, start, 0);
		}
	}
}

