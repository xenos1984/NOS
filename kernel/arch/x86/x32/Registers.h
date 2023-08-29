// Registers.h - Register dump pushed on interrupt / exception.

#ifndef __ARCH_X86_X32_REGISTERS_H__
#define __ARCH_X86_X32_REGISTERS_H__

#include <cstdint>

namespace Kernel
{
	struct RegisterSet
	{
		uint32_t edi;
		uint32_t esi;
		uint32_t ebp;
		uint32_t esp;
		uint32_t ebx;
		uint32_t edx;
		uint32_t ecx;
		uint32_t eax;
		uint32_t eip;
		uint32_t cs;
		uint32_t eflags;

		void Dump(void);
	} PACKED;

	struct SelectorErrorCode
	{
		uint32_t External : 1;
		uint32_t DescriptorIDT : 1;
		uint32_t DescriptorLDT : 1;
		uint32_t SelectorIndex : 13;
		uint32_t Reserved : 16;

		void Dump(void);
	} PACKED;

	struct PageFaultErrorCode
	{
		uint32_t Present : 1;
		uint32_t WriteAccess : 1;
		uint32_t UserMode : 1;
		uint32_t ReservedBits : 1;
		uint32_t InstructionFetch : 1;
		uint32_t Reserved : 27;

		void Dump(void);
	} PACKED;
}

#endif
