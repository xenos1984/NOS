// SystemRegs.h - Aarch32 architecture system registers.

#ifndef __ARCH_ARM_A32_SYSTEMREGS_H__
#define __ARCH_ARM_A32_SYSTEMREGS_H__

#include <cstdint>
#include INC_ARCH(Extensions.h)

#define READ_ONLY_REG_32(cp, crn, crm, op1, op2) \
	inline uint32_t Read(void) \
	{ \
		uint32_t x; \
		\
		asm volatile ("mrc " cp ", " op1 ", %0, " crn ", " crm ", " op2 : "=r"(x)); \
		return(x); \
	}

#define WRITE_ONLY_REG_32(cp, crn, crm, op1, op2) \
	inline void Write(uint32_t x) \
	{ \
		asm volatile ("mcr " cp ", " op1 ", %0, " crn ", " crm ", " op2 : : "r"(x)); \
	}

#define READ_WRITE_REG_32(cp, crn, crm, op1, op2) \
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

#define READ_ONLY_REG_64(cp, crm, op) \
	inline uint64_t Read(void) \
	{ \
		uint32_t x; \
		uint32_t y; \
		\
		asm volatile ("mrrc " cp ", " op ", %0, %1, " crm : "=r"(x), "=r"(y)); \
		return(x + (((uint64_t)y) << 32)); \
	}

#define WRITE_ONLY_REG_64(cp, crm, op) \
	inline void Write(uint64_t x) \
	{ \
		asm volatile ("mrrc " cp ", " op ", %0, %1, " crm : : "r"(x & 0xffffffff), "r"(x >> 32)); \
	}

#define READ_WRITE_REG_64(cp, crm, op) \
	inline uint64_t Read(void) \
	{ \
		uint32_t x; \
		uint32_t y; \
		\
		asm volatile ("mrrc " cp ", " op ", %0, %1, " crm : "=r"(x), "=r"(y)); \
		return(x + (((uint64_t)y) << 32)); \
	} \
	\
	inline void Write(uint64_t x) \
	{ \
		asm volatile ("mrrc " cp ", " op ", %0, %1, " crm : : "r"(x & 0xffffffff), "r"(x >> 32)); \
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
		/** Counter Frequency Register */
		namespace CNTFRQ
		{
			READ_WRITE_REG_32("p15", "c14", "c0", "0", "0")
		}

		/** Physical Count Register */
		namespace CNTPCT
		{
			READ_ONLY_REG_64("p15", "c14", "0")
		}

		/** Virtual Count Register */
		namespace CNTVCT
		{
			READ_ONLY_REG_64("p15", "c14", "1")
		}

		/** PL1 Physical Timer Compare Value Register */
		namespace CNTP_CVAL
		{
			READ_WRITE_REG_64("p15", "c14", "2")
		}

		/** Virtual Timer Compare Value Register */
		namespace CNTV_CVAL
		{
			READ_WRITE_REG_64("p15", "c14", "3")
		}

		/** Virtual Offset Register */
		namespace CNTVOFF
		{
			READ_WRITE_REG_64("p15", "c14", "4")
		}

		/** PL2 Physical Timer Compare Value Register */
		namespace CNTHP_CVAL
		{
			READ_WRITE_REG_64("p15", "c14", "6")
		}

		/** Timer PL1 Control Register */
		namespace CNTKCTL
		{
			READ_WRITE_REG_32("p15", "c14", "c1", "0", "0")
		}

		/** PL1 Physical Timer Value Register */
		namespace CNTP_TVAL
		{
			READ_WRITE_REG_32("p15", "c14", "c2", "0", "0")
		}

		/** PL1 Physical Timer Control Register */
		namespace CNTP_CTL
		{
			READ_WRITE_REG_32("p15", "c14", "c2", "0", "1")
		}

		/** Virtual Timer Value Register */
		namespace CNTV_TVAL
		{
			READ_WRITE_REG_32("p15", "c14", "c3", "0", "0")
		}

		/** Virtual Timer Control Register */
		namespace CNTV_CTL
		{
			READ_WRITE_REG_32("p15", "c14", "c3", "0", "1")
		}

		/** Timer PL2 Control Register */
		namespace CNTHCTL
		{
			READ_WRITE_REG_32("p15", "c14", "c1", "4", "0")
		}

		/** PL2 Physical Timer Value Register */
		namespace CNTHP_TVAL
		{
			READ_WRITE_REG_32("p15", "c14", "c2", "4", "0")
		}

		/** PL2 Physical Timer Control Register */
		namespace CNTHP_CTL
		{
			READ_WRITE_REG_32("p15", "c14", "c2", "4", "1")
		}

		/** Data Fault Address Register */
		namespace DFAR
		{
			READ_WRITE_REG_32("p15", "c6", "c0", "0", "0")
		}

		/** Data Fault Status Register */
		namespace DFSR
		{
			READ_WRITE_REG_32("p15", "c5", "c0", "0", "0")
		}

		/** Instruction Fault Address Register */
		namespace IFAR
		{
			READ_WRITE_REG_32("p15", "c6", "c0", "0", "2")
		}

		/** Instruction Fault Status Register */
		namespace IFSR
		{
			READ_WRITE_REG_32("p15", "c5", "c0", "0", "1")
		}

		/** Main ID Register */
		namespace MIDR
		{
			READ_ONLY_REG_32("p15", "c0", "c0", "0", "0")
		}

		/** System Control Register */
		namespace SCTLR
		{
			enum Flags : uint32_t
			{
				FLAG_MMU_EN     = 0x00000001,
				FLAG_ALIGN_EN   = 0x00000002,
				FLAG_CACHE_EN   = 0x00000004,
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

			READ_WRITE_REG_32("p15", "c1", "c0", "0", "0")
		}

		/** Invalidate entire unified TLB. */
		namespace TLBIALL
		{
			WRITE_ONLY_REG_32("p15", "c8", "c7", "0", "0")
		}

		/** Invalidate unified TLB by ASID. */
		namespace TLBIASID
		{
			WRITE_ONLY_REG_32("p15", "c8", "c7", "0", "2")
		}

		/** Invalidate unified TLB by MVA. */
		namespace TLBIMVA
		{
			WRITE_ONLY_REG_32("p15", "c8", "c7", "0", "1")
		}

		/** Invalidate unified TLB by MVA, all ASID. */
		namespace TLBIMVAA
		{
			WRITE_ONLY_REG_32("p15", "c8", "c7", "0", "3")
		}

#ifdef ARM_EXT_SMP
		/** Invalidate entire unified TLB. */
		namespace TLBIALLIS
		{
			WRITE_ONLY_REG_32("p15", "c8", "c3", "0", "0")
		}

		/** Invalidate unified TLB by ASID. */
		namespace TLBIASIDIS
		{
			WRITE_ONLY_REG_32("p15", "c8", "c3", "0", "2")
		}

		/** Invalidate unified TLB by MVA. */
		namespace TLBIMVAIS
		{
			WRITE_ONLY_REG_32("p15", "c8", "c3", "0", "1")
		}

		/** Invalidate unified TLB by MVA, all ASID. */
		namespace TLBIMVAAIS
		{
			WRITE_ONLY_REG_32("p15", "c8", "c3", "0", "3")
		}
#endif

#ifdef ARM_EXT_SECURITY
		/** Monitor Vector Base Address Register */
		namespace MVBAR
		{

			READ_WRITE_REG_32("p15", "c12", "c0", "0", "1")
		}

		/** Non-Secure Access Control Register */
		namespace NSACR
		{
			enum Flags : uint32_t
			{
				FLAG_CP0      = 0x00000001,
				FLAG_CP1      = 0x00000002,
				FLAG_CP2      = 0x00000004,
				FLAG_CP3      = 0x00000008,
				FLAG_CP4      = 0x00000010,
				FLAG_CP5      = 0x00000020,
				FLAG_CP6      = 0x00000040,
				FLAG_CP7      = 0x00000080,
				FLAG_CP8      = 0x00000100,
				FLAG_CP9      = 0x00000200,
				FLAG_CP10     = 0x00000400,
				FLAG_CP11     = 0x00000800,
				FLAG_CP12     = 0x00001000,
				FLAG_CP13     = 0x00002000,
				FLAG_NSD32DIS = 0x00004000,
				FLAG_NSASEDIS = 0x00008000,
				FLAG_RFR      = 0x00080000,
				FLAG_NSTRCDIS = 0x00100000
			};

			READ_WRITE_REG_32("p15", "c1", "c1", "0", "2")
		}

		/** Non-Secure Access Control Register */
		namespace SCR
		{
			enum Flags : uint32_t
			{
				FLAG_NS  = 0x00000001,
				FLAG_IRQ = 0x00000002,
				FLAG_FIQ = 0x00000004,
				FLAG_EA  = 0x00000008,
				FLAG_FW  = 0x00000010,
				FLAG_AW  = 0x00000020,
				FLAG_NET = 0x00000040,
				FLAG_SCD = 0x00000080,
				FLAG_HCE = 0x00000100,
				FLAG_SIF = 0x00000200
			};

			READ_WRITE_REG_32("p15", "c1", "c1", "0", "0")
		}

		/** Vector Base Address Register */
		namespace VBAR
		{
			READ_WRITE_REG_32("p15", "c12", "c0", "0", "0")
		}
#endif
	}
}

#undef READ_ONLY_REG_32
#undef WRITE_ONLY_REG_32
#undef READ_WRITE_REG_32

#endif
