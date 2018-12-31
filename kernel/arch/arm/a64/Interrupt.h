// Interrupt.h - Interrupts and exceptions in ARM architecture.

#ifndef __ARCH_ARM_A64_INTERRUPT_H__
#define __ARCH_ARM_A64_INTERRUPT_H__

#include <cstdint>

namespace Kernel
{
	struct RegisterSet
	{
		uint64_t usp;
		uint64_t ulr;
		uint64_t r0;
		uint64_t r1;
		uint64_t r2;
		uint64_t r3;
		uint64_t r4;
		uint64_t r5;
		uint64_t r6;
		uint64_t r7;
		uint64_t r8;
		uint64_t r9;
		uint64_t r10;
		uint64_t r11;
		uint64_t r12;
		uint64_t r13;
		uint64_t r14;
		uint64_t r15;
		uint64_t r16;
		uint64_t r17;
		uint64_t r18;
		uint64_t r19;
		uint64_t r20;
		uint64_t r21;
		uint64_t r22;
		uint64_t r23;
		uint64_t r24;
		uint64_t r25;
		uint64_t r26;
		uint64_t r27;
		uint64_t r28;
		uint64_t r29;
		uint64_t lr;
		uint64_t pc;
		uint64_t ps;

		void Dump(void);
	} PACKED;
}

#endif
