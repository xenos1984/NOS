// Thread.h - Threads for i386 architecture.

#ifndef __ARCH_X86_X32_THREAD_H__
#define __ARCH_X86_X32_THREAD_H__

#include <cstdint>
#include INC_ARCH(FPU.h)

namespace Kernel
{
	struct ThreadData
	{
		uint32_t eip;
		uint32_t eax;
		uint32_t ebx;
		uint32_t ecx;
		uint32_t edx;
		uint32_t esi;
		uint32_t edi;
		uint32_t ebp;
		uint32_t esp;
		uint32_t eflags;
		MMState* mmstate;

		void Dump(void);
	} PACKED;
}

#endif
