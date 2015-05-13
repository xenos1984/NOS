// VirtualMemory.h - Virtual memory management

#ifndef __VIRTUALMEMORY_H__
#define __VIRTUALMEMORY_H__

#include <PhysicalMemory.h>
#include <AtomicOps.h>

namespace Kernel
{
	struct MemoryPointer
	{
		void* mem;
		unsigned long length : (8 * sizeof(unsigned long) - 1);
		unsigned long free : 1;
		MemoryPointer* prev;
		MemoryPointer* next;
	};

	class VirtualMemory
	{
	private:
		unsigned long count, countperblock, counttotal, blocksize;
		MemoryPointer* base;
		AtomicLock lock;

		void FreeChunk(MemoryPointer* mp);
		void AllocChunk(MemoryPointer* mp);

		void Merge(MemoryPointer* mp);
		void Split(MemoryPointer* mp, unsigned long n);

	public:
		VirtualMemory(void);

		void* Alloc(unsigned long bytes, bool physalloc = true);
		void* Alloc(unsigned long bytes, unsigned long align, bool physalloc = true);
		void Free(void* addr, bool physalloc = true);

		void ShowMap(void);
	};
}

extern char virtmem_space[];
inline Kernel::VirtualMemory& virtmem(void) { return(reinterpret_cast<Kernel::VirtualMemory&>(virtmem_space)); }

#endif
