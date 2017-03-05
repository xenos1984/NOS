// Heap.h - Kernel heap management

#ifndef __HEAP_H__
#define __HEAP_H__

namespace Kernel
{
	namespace Heap
	{
		void Init(void);

		void* Alloc(unsigned long bytes, bool phys = true);
		void* Alloc(unsigned long bytes, unsigned long align, bool phys = true);
		void Free(void* addr, bool phys = true);

		void ShowMap(void);
	}
}

#endif
