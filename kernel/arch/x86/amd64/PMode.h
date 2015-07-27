// PMode.h - Some routines for protected / long mode management.

#ifndef __ARCH_X86_AMD64_PMODE_H__
#define __ARCH_X86_AMD64_PMODE_H__

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

#endif
