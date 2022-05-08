// PMode.h - Some routines for protected / long mode management.

#ifndef __ARCH_X86_AMD64_PMODE_H__
#define __ARCH_X86_AMD64_PMODE_H__

#include <cstdint>

namespace Kernel
{
	/**
	 * x86_64 Task State Segment.
	 */
	struct TSS
	{
		uint32_t pad0;
		uint64_t rsp[3];
		uint64_t pad1;
		uint64_t ist[7];
		uint64_t pad2;
		uint16_t pad3;
		uint16_t iobase;
	} PACKED;

	inline unsigned short GetTaskReg(void)
	{
		unsigned short x;

		asm volatile ("str %0" : "=r"(x));
		return(x);
	}

	inline void SetTaskReg(unsigned short x)
	{
		asm volatile("ltr %0" : : "r"(x));
	}

	inline void InvalidatePage(unsigned long addr)
	{
		asm volatile ("invlpg (%0)" : : "r"(addr));
	}

	inline void CLI(void)
	{
		asm volatile ("cli");
	}

	inline void STI(void)
	{
		asm volatile ("sti");
	}

	inline void Halt(void)
	{
		asm volatile ("hlt");
	}

	inline unsigned long ReadTimeStamp(void)
	{
		unsigned int high, low;

		/*	asm volatile ("rdtsc \n"
			"shlq $32, %%rdx \n"
			"movl %%eax, %%edx" : "=d"(value));
			*/	asm volatile ("rdtsc" : "=a"(low), "=d"(high));
		return((((unsigned long)high) << 32) + low);
	}
}

#endif
