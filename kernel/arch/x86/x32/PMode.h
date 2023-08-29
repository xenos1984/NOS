// PMode.h - Some routines for protected mode management.

#ifndef __ARCH_X86_IA32_PMODE_H__
#define __ARCH_X86_IA32_PMODE_H__

#include <cstdint>

namespace Kernel
{
	/**
	 * i386 Task State Segment.
	 */
	struct TSS
	{
		uint16_t previous;
		uint16_t pad0;
		uint32_t esp0;
		uint16_t ss0;
		uint16_t pad1;
		uint32_t esp1;
		uint16_t ss1;
		uint16_t pad2;
		uint32_t esp2;
		uint16_t ss2;
		uint16_t pad3;
		uint32_t cr3;
		uint32_t eip;
		uint32_t eflags;
		uint32_t eax;
		uint32_t ecx;
		uint32_t edx;
		uint32_t ebx;
		uint32_t esp;
		uint32_t ebp;
		uint32_t esi;
		uint32_t edi;
		uint16_t es;
		uint16_t pad4;
		uint16_t cs;
		uint16_t pad5;
		uint16_t ss;
		uint16_t pad6;
		uint16_t ds;
		uint16_t pad7;
		uint16_t fs;
		uint16_t pad8;
		uint16_t gs;
		uint16_t pad9;
		uint16_t ldt;
		uint16_t pad10;
		uint16_t trap;
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
}

#endif
