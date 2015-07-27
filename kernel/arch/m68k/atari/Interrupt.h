// Interrupt.h - Interrupt handling

#ifndef __ARCH_M68K_ATARI_INTERRUPT_H__
#define __ARCH_M68K_ATARI_INTERRUPT_H__

#include <cstdint>

#define INTERRUPT(name) \
extern "C" void name##Wrapper(void); \
extern "C" void name(RegisterSet* regs);

#define EXCEPTION(name) \
extern "C" void name##Wrapper(void); \
extern "C" void name(RegisterSetEx* regs);

namespace Kernel
{
	struct RegisterSet
	{
		uint32_t usp;
		uint32_t a6;
		uint32_t a5;
		uint32_t a4;
		uint32_t a3;
		uint32_t a2;
		uint32_t a1;
		uint32_t a0;
		uint32_t d7;
		uint32_t d6;
		uint32_t d5;
		uint32_t d4;
		uint32_t d3;
		uint32_t d2;
		uint32_t d1;
		uint32_t d0;
		uint16_t sr;
		uint32_t pc;
	} PACKED;

	struct RegisterSetEx
	{
		uint32_t usp;
		uint32_t a6;
		uint32_t a5;
		uint32_t a4;
		uint32_t a3;
		uint32_t a2;
		uint32_t a1;
		uint32_t a0;
		uint32_t d7;
		uint32_t d6;
		uint32_t d5;
		uint32_t d4;
		uint32_t d3;
		uint32_t d2;
		uint32_t d1;
		uint32_t d0;
		uint16_t flags;
		uint32_t addr;
		uint16_t ins;
		uint16_t sr;
		uint32_t pc;
	} PACKED;
}

using namespace Kernel;

EXCEPTION(BusError)
EXCEPTION(AddressError)
INTERRUPT(IllegalInstr)
INTERRUPT(DivByZero)

extern void (*intvects[256])(void);

#endif
