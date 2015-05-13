// ControlRegisters.h - x86 control registers CRn.

#ifndef __ARCH_X86_CONTROLREGISTERS_H__
#define __ARCH_X86_CONTROLREGISTERS_H__

#define READ_WRITE_REG(reg) \
	inline unsigned long Read(void) \
	{ \
		unsigned long x; \
		\
		asm volatile ("mov " reg ", %0" : "=r"(x)); \
		return(x); \
	} \
	\
	inline void Write(unsigned long x) \
	{ \
		asm volatile ("mov " "%0, " reg : : "r"(x)); \
	} \
	\
	inline void Set(unsigned long x) \
	{ \
		Write(Read() | x); \
	} \
	\
	inline void Unset(unsigned long x) \
	{ \
		Write(Read() & ~x); \
	}

namespace Kernel
{
	namespace CR0
	{
		const unsigned long PROTECTION_ENABLE    = 0x00000001;
		const unsigned long MONITOR_COPROCESSOR  = 0x00000002;
		const unsigned long EMULATION            = 0x00000004;
		const unsigned long TASK_SWITCHED        = 0x00000008;
		const unsigned long EXTENSION_TYPE       = 0x00000010;
		const unsigned long NUMERIC_ERROR        = 0x00000020;
		const unsigned long WRITE_PROTECT        = 0x00010000;
		const unsigned long ALIGNMENT_MASK       = 0x00040000;
		const unsigned long NOT_WRITE_THROUGH    = 0x20000000;
		const unsigned long CACHE_DISABLE        = 0x40000000;
		const unsigned long PAGING_ENABLE        = 0x80000000;

		READ_WRITE_REG("%%cr0")
	}

	namespace CR2
	{
		READ_WRITE_REG("%%cr2")
	}

	namespace CR3
	{
		const unsigned long WRITES_TRANSPARENT   = 0x00000008;
		const unsigned long CACHE_DISABLE        = 0x00000010;

		READ_WRITE_REG("%%cr3")
	}

	namespace CR4
	{
		const unsigned long V86_MODE_EXT_ENABLE  = 0x00000001;
		const unsigned long PMODE_VIRT_INT       = 0x00000002;
		const unsigned long TIME_STAMP_DISABLE   = 0x00000004;
		const unsigned long DEBUG_EXT_ENABLE     = 0x00000008;
		const unsigned long PAGE_SIZE_EXT_ENABLE = 0x00000010;
		const unsigned long PHYS_ADDR_EXT_ENABLE = 0x00000020;
		const unsigned long MACHINE_CHECK_ENABLE = 0x00000040;
		const unsigned long PAGE_GLOBAL_ENABLE   = 0x00000080;
		const unsigned long PERFMON_ENABLE       = 0x00000100;
		const unsigned long OS_FXSR_SUPPORT      = 0x00000200;
		const unsigned long OS_XMM_SUPPORT       = 0x00000400;
		const unsigned long VMX_ENABLE           = 0x00002000;
		const unsigned long SMX_ENABLE           = 0x00004000;
		const unsigned long PCID_ENABLE          = 0x00020000;
		const unsigned long OS_XSR_SUPPORT       = 0x00040000;

		READ_WRITE_REG("%%cr4")
	}
}

#undef READ_WRITE_REG

#endif
