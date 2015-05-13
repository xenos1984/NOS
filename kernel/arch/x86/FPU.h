// FPU.h - Basic FPU functions.

#ifndef __ARCH_X86_FPU_H__
#define __ARCH_X86_FPU_H__

#include <cstdint>

namespace Kernel
{
	struct ExtPrecision
	{
		uint64_t Fraction : 63;
		uint64_t Integer : 1;
		uint64_t Exponent : 15;
		uint64_t Sign : 1;
	} PACKED;

	struct Media128
	{
		uint64_t Low;
		uint64_t High;
	} PACKED;

	union FPUControl
	{
		struct
		{
			uint16_t InvOpMask : 1;
			uint16_t DenOpMask : 1;
			uint16_t ZeroDivMask : 1;
			uint16_t OverMask : 1;
			uint16_t UnderMask : 1;
			uint16_t PrecMask : 1;
			uint16_t space1 : 2;
			uint16_t PrecControl : 2;
			uint16_t RoundControl : 2;
			uint16_t InfControl : 1;
			uint16_t space2 : 3;
		} bits;
		uint16_t raw;
	} PACKED;

	union FPUStatus
	{
		struct
		{
			uint16_t InvOpEx : 1;
			uint16_t DenOpEx : 1;
			uint16_t ZeroDivEx : 1;
			uint16_t OverEx : 1;
			uint16_t UnderEx : 1;
			uint16_t PrecEx : 1;
			uint16_t StackFault : 1;
			uint16_t Error : 1;
			uint16_t Cond0 : 1;
			uint16_t Cond1 : 1;
			uint16_t Cond2 : 1;
			uint16_t TopOfStack : 3;
			uint16_t Cond3 : 1;
			uint16_t Busy : 1;
		} bits;
		uint16_t raw;
	} PACKED;

	struct FPUState
	{
		FPUControl ControlWord;
		uint16_t space1;
		FPUStatus StatusWord;
		uint16_t space2;
		uint16_t TagWord;
		uint16_t space3;
		uint32_t InstructionOffset;
		uint16_t InstructionSelector;
		uint16_t Opcode;
		uint32_t OperandOffset;
		uint16_t OperandSelector;
		uint16_t space4;
		ExtPrecision ST_MMX[8];

	public:
		FPUState(void);

		inline void Save(void)
		{
			asm volatile ("fsave %[mem]" : [mem] "=m" (*this) : : "memory");
		}

		inline void Restore(void)
		{
			asm volatile ("frstor %[mem]" : : [mem] "m" (*this));
		}
	} PACKED;

#ifdef ARCH_X86_IA32
	struct SSEState
	{
		FPUControl ControlWord;
		FPUStatus StatusWord;
		uint8_t TagWord;
		uint8_t space1;
		uint16_t Opcode;
		uint32_t InstructionOffset;
		uint16_t InstructionSelector;
		uint16_t space2;
		uint32_t OperandOffset;
		uint16_t OperandSelector;
		uint16_t space3;
		uint32_t MXCSR;
		uint32_t MXCSRMask;
		Media128 ST_MMX[8];
		Media128 XMM[8];
		Media128 Reserved[11];
		Media128 Available[3];

	public:
		SSEState(void);

		inline void Save(void)
		{
			asm volatile ("fxsave %[mem]" : [mem] "=m" (*this) : : "memory");
		}

		inline void Restore(void)
		{
			asm volatile ("fxrstor %[mem]" : : [mem] "m" (*this));
		}
	} PACKED;
#endif

#ifdef ARCH_X86_AMD64
	struct SSEState32
	{
		FPUControl ControlWord;
		FPUStatus StatusWord;
		uint8_t TagWord;
		uint8_t space1;
		uint16_t Opcode;
		uint32_t InstructionOffset;
		uint16_t InstructionSelector;
		uint16_t space2;
		uint32_t OperandOffset;
		uint32_t space3;
		uint32_t MXCSR;
		uint32_t MXCSRMask;
		Media128 ST_MMX[8];
		Media128 XMM[16];
		Media128 Reserved[3];
		Media128 Available[3];

	public:
		SSEState32(void);

		inline void Save(void)
		{
			asm volatile ("fxsavel %[mem]" : [mem] "=m" (*this) : : "memory");
		}

		inline void Restore(void)
		{
			asm volatile ("fxrstorl %[mem]" : : [mem] "m" (*this));
		}
	} PACKED;

	struct SSEState64
	{
		FPUControl ControlWord;
		FPUStatus StatusWord;
		uint8_t TagWord;
		uint8_t space1;
		uint16_t Opcode;
		uint64_t InstructionOffset;
		uint64_t OperandOffset;
		uint32_t MXCSR;
		uint32_t MXCSRMask;
		Media128 ST_MMX[8];
		Media128 XMM[16];
		Media128 Reserved[3];
		Media128 Available[3];

	public:
		SSEState64(void);

		inline void Save(void)
		{
			asm volatile ("fxsaveq %[mem]" : [mem] "=m" (*this) : : "memory");
		}

		inline void Restore(void)
		{
			asm volatile ("fxrstorq %[mem]" : : [mem] "m" (*this));
		}
	} PACKED;

	typedef SSEState64 SSEState;
#endif

	struct AVXState
	{
		SSEState sse;
		uint64_t XState;
		uint64_t space[7];
		Media128 YMM[16];

	public:
		AVXState(void);

		inline void Save(void)
		{
			asm volatile ("xsave %[mem]" : [mem] "=m" (*this) : "a" (0xffffffff), "d" (0xffffffff) : "memory");
		}

		inline void Restore(void)
		{
			asm volatile ("xrstor %[mem]" : : [mem] "m" (*this), "a" (0xffffffff), "d" (0xffffffff));
		}
	} PACKED;

	union MMState
	{
	private:
		FPUState fpu;
		SSEState sse;
		AVXState avx;

	public:
		MMState(void);

		void Save(void);
		void Restore(void);

		static MMState* Create(void);
	};
}

#endif
