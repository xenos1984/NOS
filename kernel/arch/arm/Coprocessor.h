// Coprocessor.h - ARM architecture coprocessors.

#ifndef __ARCH_ARM_COPROCESSOR_H__
#define __ARCH_ARM_COPROCESSOR_H__

#include <cstdint>

#define READ_WRITE_REG(cp, crn, crm, op1, op2) \
	inline uint32_t Read(void) \
	{ \
		uint32_t x; \
		\
		asm volatile ("mrc " cp ", " op1 ", %0, " crn ", " crm ", " op2 : "=r"(x)); \
		return(x); \
	} \
	\
	inline void Write(uint32_t x) \
	{ \
		asm volatile ("mcr " cp ", " op1 ", %0, " crn ", " crm ", " op2 : : "r"(x)); \
	} \
	\
	inline void Set(uint32_t x) \
	{ \
		Write(Read() | x); \
	} \
	\
	inline void Unset(uint32_t x) \
	{ \
		Write(Read() & ~x); \
	}

namespace Kernel
{
	namespace Coprocessor
	{
		namespace SCTLR
		{
			READ_WRITE_REG("p15", "c1", "c0", "0", "0")
		}
	}
}

#undef READ_WRITE_REG

#endif
