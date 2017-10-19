// Heap.h - Kernel heap management

#ifndef __HEAP_H__
#define __HEAP_H__

#include <cstddef>

namespace Kernel
{
	namespace Heap
	{
		void Init(void);

		void* Alloc(size_t bytes, bool phys = true);
		void* Alloc(size_t bytes, size_t align, bool phys = true);
		void Free(void* addr, bool phys = true);

		void ShowMap(void);
	}
}

#endif
