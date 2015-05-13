// Memory.h - Memory locations used by the kernel.

#ifndef __ARCH_X86_AMD64_MEMORY_H__
#define __ARCH_X86_AMD64_MEMORY_H__

#include <cstdint>

namespace Kernel
{
	namespace Memory
	{
		static const uintptr_t ProgramStart    = 0x0000000000000000UL;
		static const uintptr_t ProgramEnd      = 0x00007fffffffffffUL;
		static const uintptr_t LibraryStart    = 0xffff800000000000UL;
		static const uintptr_t LibraryEnd      = 0xfffffeffffffffffUL;
		static const uintptr_t SupervisorStart = 0xffffff0000000000UL;
		static const uintptr_t SupervisorEnd   = 0xffffffffffffffffUL;

		static const uintptr_t HeapStart       = 0xfffffff000000000UL;
		static const uintptr_t HeapTab         = 0xfffffffe00000000UL;
		static const uintptr_t HeapEnd         = 0xfffffffeffffffffUL;

		static const uintptr_t KernelOffset    = 0xffffffff80000000UL;
	}
}

#endif
