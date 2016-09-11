// Heap.cpp - Kernel heap management

#include <Heap.h>
#include <new>

void* operator new(size_t count)
{
	return Kernel::Heap::Alloc(count);
}

void* operator new[](size_t count)
{
	return Kernel::Heap::Alloc(count);
}

void operator delete(void* pointer)
{
	Kernel::Heap::Free(pointer);
}

void operator delete[](void* pointer)
{
	Kernel::Heap::Free(pointer);
}

void operator delete(void* pointer, size_t size)
{
	Kernel::Heap::Free(pointer);
}

void operator delete[](void* pointer, size_t size)
{
	Kernel::Heap::Free(pointer);
}
