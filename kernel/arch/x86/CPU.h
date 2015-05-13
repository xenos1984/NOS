// CPU.h - x86 CPU detection and features.

#ifndef __ARCH_X86_CPU_H__
#define __ARCH_X86_CPU_H__

#include <cstdint>

namespace Kernel
{
	class CPU
	{
	private:
		uint32_t max_std_func;
		uint32_t max_ext_func;

		union
		{
			struct
			{
				uint32_t ebx_00000000;
				uint32_t edx_00000000;
				uint32_t ecx_00000000;
			} bytes;
			uint8_t string[12];
		} vendor;

		union
		{
			struct
			{
				uint32_t eax_80000002;
				uint32_t ebx_80000002;
				uint32_t ecx_80000002;
				uint32_t edx_80000002;
				uint32_t eax_80000003;
				uint32_t ebx_80000003;
				uint32_t ecx_80000003;
				uint32_t edx_80000003;
				uint32_t eax_80000004;
				uint32_t ebx_80000004;
				uint32_t ecx_80000004;
				uint32_t edx_80000004;
			} bytes;
			uint8_t string[48];
		} brand;

		union
		{
			struct
			{
				uint32_t stepping : 4;
				uint32_t model : 4;
				uint32_t family : 4;
				uint32_t type : 2;
				uint32_t space1 : 2;
				uint32_t extmodel : 4;
				uint32_t extfamily : 8;
				uint32_t space2 : 4;
			} bits PACKED;
			uint32_t raw;
		} eax_00000001; /**< CPU signature returned in EAX by CPUID with EAX = 1 */

		union
		{
			struct
			{
				uint32_t fpu    : 1; /**< Floating Point Unit */
				uint32_t vme    : 1; /**< Virtual 8086 Mode Enhancement */
				uint32_t de     : 1; /**< Debugging Extensions */
				uint32_t pse    : 1; /**< Page Size Extensions */
				uint32_t tsc    : 1; /**< Time Stamp Counter */
				uint32_t msr    : 1; /**< Model Specific Registers */
				uint32_t pae    : 1; /**< Physical Address Extensions */
				uint32_t mce    : 1; /**< Machine Check Exception */
				uint32_t cx8    : 1; /**< CMPXCHG8B Instruction */
				uint32_t apic   : 1; /**< APIC on Chip */
				uint32_t space1 : 1;
				uint32_t sep    : 1; /**< SYSENTER and SYSEXIT */
				uint32_t mtrr   : 1; /**< Memory Type Range Registers */
				uint32_t pge    : 1; /**< PTE Global Bit */
				uint32_t mca    : 1; /**< Machine Check Architecture */
				uint32_t cmov   : 1; /**< Conditional Move / Compare Instruction */
				uint32_t pat    : 1; /**< Page Attribute Table */
				uint32_t pse36  : 1; /**< Page Size Extension */
				uint32_t psn    : 1; /**< Processor Serial Number */
				uint32_t cflsh  : 1; /**< CFLUSH Instruction */
				uint32_t space2 : 1;
				uint32_t ds     : 1; /**< Debug Store */
				uint32_t acpi   : 1; /**< Thermal Monitor and Clock Control */
				uint32_t mmx    : 1; /**< Multimedia Extensions */
				uint32_t fxsr   : 1; /**< FXSAVE / FXRSTOR */
				uint32_t sse    : 1; /**< Streaming SIMD Extensions */
				uint32_t sse2   : 1; /**< Streaming SIMD Extensions 2 */
				uint32_t ss     : 1; /**< Self Snoop */
				uint32_t htt    : 1; /**< Hyper Threading Technology */
				uint32_t tm     : 1; /**< Thermal Monitor */
				uint32_t space3 : 1;
				uint32_t pbe    : 1; /**< Pending Break Enable */
			} bits PACKED;
			uint32_t raw;
		} edx_00000001; /**< Feature bits returned in EDX by CPUID with EAX = 1 */

		union
		{
			struct
			{
				uint32_t sse3   : 1; /**< Streaming SIMD Extensions 3 */
				uint32_t pclmul : 1; /**< PCLMULQDQ Instruction */
				uint32_t dtes64 : 1; /**< Debug Store Area 64 Bit Layout */
				uint32_t mwait  : 1; /**< MONITOR / MWAIT Instructions */
				uint32_t dscpl  : 1; /**< CPL Qualified Debug Store */
				uint32_t vmx    : 1; /**< Virtual Machine Extensions */
				uint32_t smx    : 1; /**< Safer Mode Extensions */
				uint32_t est    : 1; /**< Enhanced Intel SpeedStep Technology */
				uint32_t tm2    : 1; /**< Thermal Monitor 2 */
				uint32_t ssse3  : 1; /**< Supplemental Streaming SIMD Extensions 3 */
				uint32_t cnxtid : 1; /**< L1 Context ID */
				uint32_t space2 : 1;
				uint32_t fma    : 1; /**< FMA Instruction */
				uint32_t cx16   : 1; /**< CMPXCHG16B Instruction */
				uint32_t xtpr   : 1; /**< xTPR Update Control */
				uint32_t pdcm   : 1; /**< Performance / Debug Capability MSR */
				uint32_t space3 : 2;
				uint32_t dca    : 1; /**< Direct Cache Access */
				uint32_t sse4_1 : 1; /**< Streaming SIMD Extensions 4.1 */
				uint32_t sse4_2 : 1; /**< Streaming SIMD Extensions 4.2 */
				uint32_t x2apic : 1; /**< xAPIC on Chip */
				uint32_t movbe  : 1; /**< MOVBE Instruction */
				uint32_t popcnt : 1; /**< POPCNT Instruction */
				uint32_t space4 : 1;
				uint32_t aes    : 1; /**< AES Instructions */
				uint32_t xsr    : 1; /**< XSAVE / XRSTOR Instructions */
				uint32_t osxsr  : 1; /**< XSAVE / XRSTOR Enabled */
				uint32_t avx    : 1; /**< AVX Instructions */
				uint32_t f16c   : 1; /**< Half-Precision Convert Instruction */
				uint32_t space5 : 2;
			} bits PACKED;
			uint32_t raw;
		} ecx_00000001; /**< Feature bits returned in ECX by CPUID with EAX = 1 */

		union
		{
			struct
			{
				uint8_t brandid;
				uint8_t clflush;
				uint8_t logcount;
				uint8_t apicid;
			} bits;
			uint32_t raw;
		} ebx_00000001;

		uint32_t eax_0000000d;
		uint32_t edx_0000000d;
		uint32_t ecx_0000000d;
		uint32_t ebx_0000000d;

		union
		{
			struct
			{
				uint32_t space1 : 11;
				uint32_t syscr  : 1; /**< SYSCALL / SYSRET Instructions */
				uint32_t space2 : 8;
				uint32_t xd     : 1; /**< Execute Disable */
				uint32_t space3 : 1;
				uint32_t mmxx   : 1; /**< AMD MMX Extensions */
				uint32_t space4 : 2;
				uint32_t ffxsr  : 1; /**< FXSAVE / FXRESTORE Instruction Optimizations */
				uint32_t pg1gb  : 1; /**< 1 GB Pages */
				uint32_t rdtscp : 1; /**< RDTSCP Instruction */
				uint32_t space5 : 1;
				uint32_t lm     : 1; /**< Long Mode */
				uint32_t d3nowx : 1; /**< AMD Extensions to 3DNow! Instructions */
				uint32_t d3now  : 1; /**< 3DNow! Instructions */
			} bits PACKED;
			uint32_t raw;
		} edx_80000001;

		union
		{
			struct
			{
				uint32_t lsahf  : 1; /**< LAHF / SAHF Instructions */
				uint32_t cmpleg : 1; /**< Core Multi-Processing Legacy Mode */
				uint32_t svm    : 1; /**< Secure Virtual Machine */
				uint32_t xapics : 1; /**< Extended APIC Space */
				uint32_t amvcr8 : 1; /**< Alternative MOV CR8 Instruction */
				uint32_t abm    : 1; /**< Advanced Bit manipulation */
				uint32_t sse4a  : 1; /**< Streaming SIMD Extensions 4a */
				uint32_t masse  : 1; /**< Misaligned SSE Mode */
				uint32_t d3pre  : 1; /**< 3DNow Prefetch Instructions */
				uint32_t osvw   : 1; /**< OS Visible Workaround */
				uint32_t ibs    : 1; /**< Instruction Based Sampling */
				uint32_t xop    : 1; /**< Extended Operation */
				uint32_t skinit : 1; /**< SKINIT / STGI Instructions */
				uint32_t wdt    : 1; /**< Watchdog Timer */
				uint32_t space1 : 1;
				uint32_t lwp    : 1; /**< Lightweight Profiling */
				uint32_t fma4   : 1; /**< 4-operand FMA Instruction */
				uint32_t space2 : 2;
				uint32_t nodeid : 1; /**< NodeID MSR */
				uint32_t space3 : 1;
				uint32_t tbm    : 1; /**< Trailing Bit Manipulation Instruction */
				uint32_t topx   : 1; /**< Topology Extensions */
				uint32_t space4 : 9;
			} bits PACKED;
			uint32_t raw;
		} ecx_80000001;

		union
		{
			struct
			{
				uint16_t brandid;
				uint32_t space1 : 12;
				uint32_t pkgtype : 4;
			} bits PACKED;
			uint32_t raw;
		} ebx_80000001;

	public:
		CPU(void);

		inline bool HasAPIC(void) const
		{
			return(edx_00000001.bits.apic);
		}

		inline bool HasFPU(void) const
		{
			return(edx_00000001.bits.fpu);
		}

		inline bool HasFXSR(void) const
		{
			return(edx_00000001.bits.fxsr);
		}

		inline bool HasLongMode(void) const
		{
			return(edx_80000001.bits.lm);
		}

		inline bool HasPAE(void) const
		{
			return(edx_00000001.bits.pae);
		}

		inline bool HasPGE(void) const
		{
			return(edx_00000001.bits.pge);
		}

		inline bool HasPSE(void) const
		{
			return(edx_00000001.bits.pse);
		}

		inline bool HasXSR(void) const
		{
			return(ecx_00000001.bits.xsr);
		}

		inline uint8_t GetPhysID(void) const
		{
			return(ebx_00000001.bits.apicid);
		}

		inline uint32_t GetSaveAreaSize(void) const
		{
			return(ecx_0000000d);
		}
	};
}

/** Space for BSP CPU object. */
extern char bspcpu_space[];

/** Returns a reference to the BSP CPU object. */
inline const Kernel::CPU& bspcpu(void) { return(reinterpret_cast<Kernel::CPU&>(bspcpu_space)); }

#endif
