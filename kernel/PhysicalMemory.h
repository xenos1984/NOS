// PhysicalMemory.h - Physical memory management

#ifndef __PHYSICALMEMORY_H__
#define __PHYSICALMEMORY_H__

namespace Kernel
{
	class PhysicalMemory
	{
	public:
		virtual unsigned long GetBlockSize(void) = 0;

		virtual void* AllocBlocks(void* addr, unsigned long n) = 0;
		virtual void FreeBlocks(void* addr, unsigned long n) = 0;

		virtual bool VirtExists(void* addr) = 0;
		virtual unsigned long VirtToPhys(void* addr) = 0;
		virtual void* MapToLinear(void* phys, void* virt, unsigned long n) = 0;

		virtual void CleanInit(void) = 0;
	};
}

extern char physmem_space[];
inline Kernel::PhysicalMemory& physmem(void) { return(reinterpret_cast<Kernel::PhysicalMemory&>(physmem_space)); }

#endif
