// SMP.h - Symmetric multiprocessing management.

#ifndef __ARCH_X86_SMP_H__
#define __ARCH_X86_SMP_H__

#include <cstdint>

#ifdef CONFIG_SMP
namespace Kernel
{
	class SMP
	{
	public:
		/** SMP floating pointer structure. */
		struct FloatingPointer
		{
			uint32_t Magic;     /**< Floating pointer magic. */
			uint32_t Table;     /**< Physical address of the config table, if any. */
			uint8_t Length;     /**< Length of this structure in paragraphs (16 bytes). */
			uint8_t Version;    /**< Version of the MP specification. */
			uint8_t Checksum;   /**< Floating pointer checksum. */
			uint8_t Feature[5]; /**< Feature information bytes. */
		} PACKED;

		/** SMP configuration table header. */
		struct ConfigTable
		{
			uint32_t Magic;            /**< Table header magic. */
			uint16_t BaseLength;       /**< Length of the base table. */
			uint8_t Version;           /**< Version of the MP specification. */
			uint8_t Checksum;          /**< Base table checksum. */
			uint8_t OemString[8];      /**< Hardware OEM string. */
			uint8_t ProductString[12]; /**< Hardware product ID. */
			uint32_t OemTable;         /**< Physical address of an OEM defined table. */
			uint16_t OemLength;        /**< Length of the OEM table. */
			uint16_t EntryCount;       /**< Number of entries in the config table. */
			uint32_t LocalApic;        /**< Physical address of the local APIC. */
			uint16_t ExtendedLength;   /**< Length of the extended table. */
			uint8_t ExtendedChecksum;  /**< Extended table checksum. */
			uint8_t Reserved;
		} PACKED;

		/** SMP processor entry. */
		struct Cpu
		{
			uint8_t Type;             /**< Entry type (processor). */
			uint8_t LocalApicID;      /**< Local APIC ID. */
			uint8_t LocalApicVersion; /**< Version of the local APIC. */
			uint8_t Flags;            /**< CPU flags. */
			uint32_t Signature;       /**< CPU signature (stepping, model, family). */
			uint32_t Features;        /**< CPU feature flags. */
			uint32_t Reserved[2];
		} PACKED;

		/** SMP bus entry. */
		struct Bus
		{
			uint8_t Type;          /**< Entry type (bus). */
			uint8_t ID;            /**< Bus ID, zero based. */
			uint8_t TypeString[6]; /**< Bus type string. */
		} PACKED;

		/** SMP I/O APIC entry. */
		struct IOApic
		{
			uint8_t Type;        /**< Entry type (I/O APIC). */
			uint8_t ApicID;      /**< I/O APIC ID. */
			uint8_t ApicVersion; /**< I/O APIC version. */
			uint8_t Flags;       /**< I/O APIC flags. */
			uint32_t Address;    /**< Physical base address. */
		} PACKED;

		/** SMP interrupt entry. */
		struct Interrupt
		{
			uint8_t Type;      /**< Entry type (I/O or local interrupt). */
			uint8_t IntType;   /**< Interrupt type. */
			uint16_t Flags;    /**< Interrupt flags. */
			uint8_t SourceID;  /**< Source bus ID. */
			uint8_t SourceIRQ; /**< Source bus IRQ. */
			uint8_t DestID;    /**< Destination APIC ID. */
			uint8_t DestIRQ;   /**< Destination APIC IRQ. */
		} PACKED;

		/** System address space mapping. */
		struct AddressSpace
		{
			uint8_t Type;           /**< Entry type (address space). */
			uint8_t Length;         /**< Entry length (20). */
			uint8_t BusID;          /**< Bus ID. */
			uint8_t AddressType;    /**< System address type. */
			uint64_t AddressBase;   /**< Base address. */
			uint64_t AddressLength; /**< Address space length. */
		} PACKED;

		/** Bus hierarchy descriptor. */
		struct BusHierarchy
		{
			uint8_t Type;        /**< Entry type (bus hierarchy). */
			uint8_t Length;      /**< Entry length (8). */
			uint8_t BusID;       /**< Child bus ID. */
			uint8_t BusInfo;     /**< Bus info flags. */
			uint8_t ParentBusID; /**< Parent bus ID. */
			uint8_t Reserved[3];
		} PACKED;

		/** Compatibility bus address space modifier. */
		struct Compatibility
		{
			uint8_t Type;       /**< Entry type (compatibility). */
			uint8_t Length;     /**< Entry length (8). */
			uint8_t BusID;      /**< Bus ID. */
			uint8_t AddressMod; /**< Address modifier. */
			uint32_t RangeList; /**< Predefined address range list. */
		} PACKED;

	private:
		int numcpus, numbuses, numioapics, numints, numlints;
		Cpu** cpus;
		Bus** buses;
		IOApic** ioapics;
		Interrupt** ints;
		Interrupt** lints;

		FloatingPointer* pointer;
		ConfigTable* table;

		SMP(FloatingPointer* sfp);

	public:
		static const uint32_t POINTER_MAGIC  = 0x5f504d5f; //'_MP_'
		static const uint32_t TABLE_MAGIC    = 0x504d4350; //'PCMP'

		static const uint8_t ENTRY_PROCESSOR = 0x00;
		static const uint8_t ENTRY_BUS       = 0x01;
		static const uint8_t ENTRY_IO_APIC   = 0x02;
		static const uint8_t ENTRY_IO_IRQ    = 0x03;
		static const uint8_t ENTRY_LOCAL_IRQ = 0x04;
		static const uint8_t ENTRY_SPACE     = 0x80;
		static const uint8_t ENTRY_HIERARCHY = 0x81;
		static const uint8_t ENTRY_COMPAT    = 0x82;

		static const uint8_t INT_VECTORED    = 0x00;
		static const uint8_t INT_NMI         = 0x01;
		static const uint8_t INT_SMI         = 0x02;
		static const uint8_t INT_EXTERNAL    = 0x03;

		static const uint8_t CPU_ENABLED     = 0x01;
		static const uint8_t CPU_BSP         = 0x02;

		static const uint8_t IMCR_PRESENT    = 0x80;

		static bool Init(unsigned long first, unsigned long last);

		inline int GetProcessorCount(void)
		{
			return numcpus;
		}

		inline Cpu* GetProcessor(int n)
		{
			return cpus[n];
		}

		inline int GetIOApicCount(void)
		{
			return numioapics;
		}

		inline IOApic* GetIOApic(int n)
		{
			return ioapics[n];
		}

		inline bool HasIMCR(void)
		{
			return(pointer->Feature[1] & IMCR_PRESENT);
		}

		int GetISAIRQ(int n);
	};
}
#endif

extern char smp_space[];
inline Kernel::SMP& smp(void) { return(reinterpret_cast<Kernel::SMP&>(smp_space)); }

#endif
