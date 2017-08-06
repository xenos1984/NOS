// Heap.h - Kernel heap management

#ifndef __HEAP_HEAP_H__
#define __HEAP_HEAP_H__

#include <Heap.h>
#include <Memory.h>
#include <cstdint>

namespace Kernel
{
	namespace Heap
	{
		inline uintptr_t RoundUp(uintptr_t addr, uintptr_t align = sizeof(long))
		{
			return ((addr - 1) & (-align)) + align;
		}

		inline uintptr_t RoundDown(uintptr_t addr, uintptr_t align = sizeof(long))
		{
			return addr & (-align);
		}

		inline uintptr_t RoundDiff(uintptr_t addr, uintptr_t align = sizeof(long))
		{
			return align - 1 - ((addr - 1) & (align - 1));
		}
	}
}

#endif

