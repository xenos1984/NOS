// Registers.h - Register dump pushed on interrupt / exception.

#ifndef __ARCH_X86_AMD64_REGISTERS_H__
#define __ARCH_X86_AMD64_REGISTERS_H__

#include <cstdint>

namespace Kernel
{
	struct RegisterSet
	{
		uint64_t r15;
		uint64_t r14;
		uint64_t r13;
		uint64_t r12;
		uint64_t r11;
		uint64_t r10;
		uint64_t r9;
		uint64_t r8;
		uint64_t rdi;
		uint64_t rsi;
		uint64_t rbp;
		uint64_t rsp;
		uint64_t rbx;
		uint64_t rdx;
		uint64_t rcx;
		uint64_t rax;
		uint64_t rip;
		uint64_t cs;
		uint64_t rflags;

		void Dump(void);
	} PACKED;

	struct SelectorErrorCode
	{
		uint64_t External : 1;
		uint64_t DescriptorIDT : 1;
		uint64_t DescriptorLDT : 1;
		uint64_t SelectorIndex : 13;
		uint64_t Reserved : 48;

		void Dump(void);
	} PACKED;

	struct PageFaultErrorCode
	{
		uint64_t Present : 1;
		uint64_t WriteAccess : 1;
		uint64_t UserMode : 1;
		uint64_t ReservedBits : 1;
		uint64_t InstructionFetch : 1;
		uint64_t Reserved : 59;

		void Dump(void);
	} PACKED;
}

#endif
