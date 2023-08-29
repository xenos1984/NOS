// Memory.h - General memory information.

#ifndef __ARCH_X86_X32_MEMORY_H__
#define __ARCH_X86_X32_MEMORY_H__

#include <cstdint>

namespace Kernel
{
	namespace Memory
	{
#ifdef IA32_PAE
		typedef uint64_t PhysAddr;
#else
		typedef uint32_t PhysAddr;
#endif
	}
}

#endif
