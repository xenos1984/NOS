// Interrupt.h - Interrupts and exceptions in ARM architecture.

#ifndef __ARCH_ARM_INTERRUPT_H__
#define __ARCH_ARM_INTERRUPT_H__

#include <cstdint>

namespace Kernel
{
	struct RegisterSet
	{
		uint32_t usp;
		uint32_t ulr;
		uint32_t r0;
		uint32_t r1;
		uint32_t r2;
		uint32_t r3;
		uint32_t r4;
		uint32_t r5;
		uint32_t r6;
		uint32_t r7;
		uint32_t r8;
		uint32_t r9;
		uint32_t r10;
		uint32_t r11;
		uint32_t r12;
		uint32_t lr;
		uint32_t pc;
		uint32_t spsr;

		void Dump(void);
	} PACKED;
}

#endif
