// Thread.h - Threads for x64 architecture.

#ifndef __ARCH_X86_X64_THREAD_H__
#define __ARCH_X86_X64_THREAD_H__

#include <cstdint>
#include INC_ARCH(FPU.h)

namespace Kernel
{
	struct ThreadData
	{
		uint64_t rip;
		uint64_t rax;
		uint64_t rbx;
		uint64_t rcx;
		uint64_t rdx;
		uint64_t rsi;
		uint64_t rdi;
		uint64_t rbp;
		uint64_t rsp;
		uint64_t r8;
		uint64_t r9;
		uint64_t r10;
		uint64_t r11;
		uint64_t r12;
		uint64_t r13;
		uint64_t r14;
		uint64_t r15;
		uint64_t rflags;
		MMState* mmstate;

		void Dump(void);
	} PACKED;
}

#endif
