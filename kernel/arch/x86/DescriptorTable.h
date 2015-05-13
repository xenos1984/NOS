// DescriptorTable.h - Handling descriptor tables

#ifndef __ARCH_X86_DESCRIPTORTABLE_H__
#define __ARCH_X86_DESCRIPTORTABLE_H__

#include <cstdint>

namespace Kernel
{
	class DescriptorTable
	{
	private:
		struct Segment
		{
			uint16_t LimitLow;       /**< Segment limit 0-15 */
			uint16_t BaseLow;        /**< Segment base 0-15 */
			uint8_t BaseMed;         /**< Segment base 16-23 */
			uint32_t Accessed:1;     /**< Segment was accessed */
			uint32_t ReadWrite:1;    /**< Readable code segment / writable data segment */
			uint32_t ConfExpDown:1;  /**< Conforming code segment / expand-down data segment */
			uint32_t CodeSegment:1;  /**< Code segment */
			uint32_t CodeOrData:1;   /**< Code or data segment */
			uint32_t Privilege:2;    /**< Descriptor privilege level */
			uint32_t Present:1;      /**< Present */
			uint32_t LimitHigh:4;    /**< Segment limit 16-19 */
			uint32_t Available:1;    /**< Available for OS use */
			uint32_t Segment64Bit:1; /**< 64 bit segment */
			uint32_t OpSize32Bit:1;  /**< Operation size 32 bit */
			uint32_t Granularity:1;  /**< Granularity 4kByte */
			uint8_t BaseHigh;        /**< Segment base 24-31 */
		} PACKED;

		struct System
		{
			uint16_t LimitLow;       /**< Segment limit 0-15 */
			uint16_t BaseLow;        /**< Segment base 0-15 */
			uint8_t BaseMed;         /**< Segment base 16-23 */
			uint32_t Type:4;         /**< Segment type */
			uint32_t CodeOrData:1;   /**< Code or data segment */
			uint32_t Privilege:2;    /**< Descriptor privilege level */
			uint32_t Present:1;      /**< Present */
			uint32_t LimitHigh:4;    /**< Segment limit 16-19 */
			uint32_t Available:1;    /**< Available for OS use */
			uint32_t Reserved:2;
			uint32_t Granularity:1;  /**< Granularity 4kByte */
			uint8_t BaseHigh;        /**< Segment base 24-31 */
#ifdef ELF64
			uint32_t BaseHigh2;      /**< Segment base 32-63 */
			uint32_t Reserved2;
#endif
		} PACKED;

		struct Gate
		{
			uint16_t OffsetLow;      /**< ISR offset 0-15 */
			uint16_t Selector;       /**< Selector index */
			uint8_t ParamIST;        /**< Parameter count / IST index */
			uint32_t Type:4;         /**< Segment type */
			uint32_t CodeOrData:1;   /**< Code or data segment */
			uint32_t Privilege:2;    /**< Descriptor privilege level */
			uint32_t Present:1;      /**< Present */
			uint16_t OffsetMed;      /**< ISR offset 16-31 */
#ifdef ELF64
			uint32_t OffsetHigh;     /**< ISR offset 32-63 */
			uint32_t Reserved;
#endif
		} PACKED;

		static const uint32_t D_TSSA286    = 0x1; /**< Task State Segment (286 style, available) */
		static const uint32_t D_LDT        = 0x2; /**< Local Descriptor Table */
		static const uint32_t D_TSSB286    = 0x3; /**< Task State Segment (286 style, busy) */
		static const uint32_t D_CALL286    = 0x4; /**< Call Gate (286 style) */
		static const uint32_t D_TASK       = 0x5; /**< Task Gate */
		static const uint32_t D_INT286     = 0x6; /**< Interrupt Gate (286 style) */
		static const uint32_t D_TRAP286    = 0x7; /**< Trap Gate (286 style) */
		static const uint32_t D_TSSA386    = 0x9; /**< Task State Segment (386 style, available) */
		static const uint32_t D_TSSB386    = 0xb; /**< Task State Segment (386 style, busy) */
		static const uint32_t D_CALL386    = 0xc; /**< Call Gate (386 style) */
		static const uint32_t D_INT386     = 0xe; /**< Interrupt Gate (386 style) */
		static const uint32_t D_TRAP386    = 0xf; /**< Trap Gate (386 style) */

		union
		{
			Segment segment[];
			System system[];
			Gate gate[];
			uint64_t raw[];
		};

	public:
		DescriptorTable(void);

		void Clear(int sel);
		void CreateInterruptGate(int sel, uint16_t cs, void (*ip)(void), uint8_t dpl = 0);
		void CreateTrapGate(int sel, uint16_t cs, void (*ip)(void), uint8_t dpl = 0);
		void CreateTSS(int sel, void* tss, uint32_t length);

		void InitIDT(void);
	};
}

extern Kernel::DescriptorTable tabIDT;
extern Kernel::DescriptorTable tabGDT;

#endif
