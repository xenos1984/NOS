// SystemRegs.h - Aarch64 architecture system registers.

#ifndef __ARCH_ARM_A64_SYSTEMREGS_H__
#define __ARCH_ARM_A64_SYSTEMREGS_H__

#include <cstdint>
#include INC_ARCH(Extensions.h)

#define READ_ONLY_REG(name) \
	inline uint64_t Read(void) \
	{ \
		uint64_t x; \
		\
		asm volatile ("mrs  %0, " name : "=r"(x)); \
		return(x); \
	}

#define WRITE_ONLY_REG(name) \
	inline void Write(uint64_t x) \
	{ \
		asm volatile ("msr " name ", %0" : : "r"(x)); \
	}

#define READ_WRITE_REG(cp, crn, crm, op1, op2) \
	inline uint64_t Read(void) \
	{ \
		uint64_t x; \
		\
		asm volatile ("mrs  %0, " name : "=r"(x)); \
		return(x); \
	} \
	\
	inline void Write(uint64_t x) \
	{ \
		asm volatile ("msr " name ", %0" : : "r"(x)); \
	} \
	\
	inline void Set(uint64_t x) \
	{ \
		Write(Read() | x); \
	} \
	\
	inline void Unset(uint64_t x) \
	{ \
		Write(Read() & ~x); \
	}

namespace Kernel
{
	namespace Sysreg
	{
		/** Clock Frequency Register */
		namespace CNTFRQ_EL0
		{
			READ_WRITE_REG("CNTFRQ_EL0")
		}

		/** Main ID Register */
		namespace MIDR_EL1
		{
			READ_ONLY_REG("MIDR_EL1")
		}

		/** System Control Register */
		namespace SCTLR_EL1
		{
			enum Flags : uint64_t
			{
				FLAG_MMU_EN     = 0x00000001,
				FLAG_ALIGN_EN   = 0x00000002,
				FLAG_CACHE_EN   = 0x00000004,
				FLAG_SPALIGN_EN = 0x00000008,
				FLAG_SPAL0_EN   = 0x00000010,
				FLAG_BARRIER_EN = 0x00000020,
				FLAG_BIG_ENDIAN = 0x00000080,
				FLAG_SWAP_EN    = 0x00000400,
				FLAG_BRANCH_EN  = 0x00000800,
				FLAG_ICACHE_EN  = 0x00001000,
				FLAG_VEC_HIGH   = 0x00002000,
				FLAG_ROBIN      = 0x00004000,
				FLAG_HWACCESS   = 0x00020000,
				FLAG_WNOEXEC    = 0x00080000,
				FLAG_UWNOEXEC   = 0x00100000,
				FLAG_FAST_IRQ   = 0x00200000,
				FLAG_UNALIGNED  = 0x00400000,
				FLAG_VECTORS_EN = 0x01000000,
				FLAG_EXC_BIG    = 0x02000000,
				FLAG_NM_FIQ     = 0x08000000,
				FLAG_TEX_REMAP  = 0x10000000,
				FLAG_ACCESS     = 0x20000000,
				FLAG_EXC_THUMB  = 0x40000000
			};

			READ_WRITE_REG("SCTLR_EL1")
		}

		/** Vector Base Address Register */
		namespace VBAR_EL1
		{
			READ_WRITE_REG("VBAR_EL1")
		}
	}
}

#undef READ_ONLY_REG
#undef WRITE_ONLY_REG
#undef READ_WRITE_REG

#endif
