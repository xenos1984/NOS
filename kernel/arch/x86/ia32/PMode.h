// PMode.h - Some routines for protected mode management.

#ifndef __ARCH_X86_IA32_PMODE_H__
#define __ARCH_X86_IA32_PMODE_H__

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

#endif
