// Memory.h - Memory locations used by the kernel.

#ifndef __ARCH_X86_IA32_MEMORY_H__
#define __ARCH_X86_IA32_MEMORY_H__

namespace Kernel
{
	namespace Memory
	{
		static const uintptr_t ProgramStart = 0x00000000UL;
		static const uintptr_t ProgramEnd = 0x7fffffffUL;
		static const uintptr_t LibraryStart = 0x80000000UL;
		static const uintptr_t LibraryEnd = 0xbfffffffUL;
		static const uintptr_t SupervisorStart = 0xc0000000UL;
		static const uintptr_t SupervisorEnd = 0xffffffffUL;

		static const uintptr_t HeapStart       = 0xd0000000UL;
		static const uintptr_t HeapTab         = 0xdf000000UL;
		static const uintptr_t HeapEnd         = 0xdfffffffUL;

		static const uintptr_t KernelOffset    = 0xc0000000UL;
	}
}

#endif
