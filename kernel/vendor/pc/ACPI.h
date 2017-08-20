// ACPI.h - Advanced Configuration and Power Interface.

#ifndef __ARCH_X86_ACPI_H__
#define __ARCH_X86_ACPI_H__

#include <cstdint>
#include <Memory.h>

#ifdef CONFIG_ACPI
namespace Kernel
{
	namespace ACPI
	{
		/** Generic address structure. */
		struct ExtendedAddress
		{
			uint8_t AddressSpace;
			uint8_t BitWidth;
			uint8_t BitOffset;
			uint8_t AccessSize;
			uint64_t Address;
		} PACKED;

		/** System description table header. */
		struct TableHeader
		{
			union
			{
				uint8_t Bytes[4];
				uint32_t Number;
			} Signature;
			uint32_t Length;
			uint8_t Revision;
			uint8_t Checksum;
			uint8_t OEMID[6];
			uint8_t OEMTabID[8];
			uint32_t OEMRev;
			uint8_t CreatorID[4];
			uint32_t CreatorRev;
		} PACKED;

		/** Root system description pointer. */
		struct RsdPtr
		{
			uint8_t Signature[8];
			uint8_t Checksum;
			uint8_t OEMID[6];
			uint8_t Revision;
			uint32_t RsdtAddress;
			uint32_t Length;
			uint64_t XsdtAddress;
			uint8_t ExtChecksum;
			uint8_t Reserved[3];
		} PACKED;

		/** Root system description table. */
		struct RsdTable
		{
			TableHeader Header;
			uint32_t Entry[];
		} PACKED;

		/** Extended system description table. */
		struct XsdTable
		{
			TableHeader Header;
			uint64_t Entry[];
		} PACKED;

		/** Fixed ACPI description table. */
		struct FadTable
		{
			TableHeader Header;
			uint32_t FacsAddress;
			uint32_t DsdtAddress;
			uint8_t IntModel;
			uint8_t PowerProfile;
			uint16_t SciInt;
			uint32_t SmiCmd;
			uint8_t AcpiEnable;
			uint8_t AcpiDisable;
			uint8_t S4Bios;
			uint8_t PState;
			uint32_t PM1aEventBlock;
			uint32_t PM1bEventBlock;
			uint32_t PM1aCountBlock;
			uint32_t PM1bCountBlock;
			uint32_t PM2CountBlock;
			uint32_t PMTimerBlock;
			uint32_t GPE0Block;
			uint32_t GPE1Block;
			uint8_t PM1EventLength;
			uint8_t PM1CountLength;
			uint8_t PM2CountLength;
			uint8_t PMTimerLength;
			uint32_t GPE0BlockLength;
			uint32_t GPE1BlockLength;
			uint8_t GPE1Base;
			uint8_t CstCount;
			uint16_t PLevel2Latency;
			uint16_t PLevel3Latency;
			uint16_t FlushSize;
			uint16_t FlushStride;
			uint8_t DutyOffset;
			uint8_t DutyWidth;
			uint8_t DayAlarm;
			uint8_t MonthAlarm;
			uint8_t Century;
			uint16_t BootArch;
			uint8_t Reserved1;
			uint32_t Flags;
			ExtendedAddress ResetReg;
			uint8_t ResetValue;
			uint8_t Reserved2[3];
			uint64_t XFacsAddress;
			uint64_t XDsdtAddress;
			ExtendedAddress XPM1aEventBlock;
			ExtendedAddress XPM1bEventBlock;
			ExtendedAddress XPM1aCountBlock;
			ExtendedAddress XPM1bCountBlock;
			ExtendedAddress XPM2CountBlock;
			ExtendedAddress XPMTimerBlock;
			ExtendedAddress XGPE0Block;
			ExtendedAddress XGPE1Block;
		} PACKED;

		/** Firmware ACPI control structure. */
		struct Facs
		{
			uint8_t Signature[4];
			uint32_t Length;
			uint32_t HardwareSignature;
			uint32_t FirmwareWaking;
			uint32_t GlobalLock;
			uint32_t Flags;
			uint64_t XFirmwareWaking;
			uint8_t Version;
		} PACKED;

		/** Multiple APIC description table. */
		struct MadTable
		{
			TableHeader Header;
			uint32_t ApicAddress;
			uint32_t Flags;
		} PACKED;

		/** Local APIC entry in the MADT. */
		struct LApic
		{
			uint8_t Type;
			uint8_t Length;
			uint8_t ProcessorID;
			uint8_t ApicID;
			uint32_t Flags;
		} PACKED;

		/** I/O APIC entry in the MADT. */
		struct IOApic
		{
			uint8_t Type;
			uint8_t Length;
			uint8_t ApicID;
			uint8_t Reserved;
			uint32_t Address;
			uint32_t InterruptBase;
		} PACKED;

		/** Interrupt source override entry in the MADT. */
		struct IrqSourceOverride
		{
			uint8_t Type;
			uint8_t Length;
			uint8_t Bus;
			uint8_t Source;
			uint32_t Interrupt;
			uint16_t Flags;
		} PACKED;

		/** Non-maskable interrupt entry in the MADT. */
		struct NMI
		{
			uint8_t Type;
			uint8_t Length;
			uint16_t Flags;
			uint32_t Interrupt;
		} PACKED;

		/** Local APIC non-maskable interrupt entry in the MADT. */
		struct LApicNMI
		{
			uint8_t Type;
			uint8_t Length;
			uint8_t ProcessorID;
			uint16_t Flags;
			uint8_t LINT;
		} PACKED;

		/** Local APIC address override entry in the MADT. */
		struct LApicAddressOverride
		{
			uint8_t Type;
			uint8_t Length;
			uint16_t Reserved;
			uint64_t Address;
		} PACKED;

		/** I/O SAPIC entry in the MADT. */
		struct IOSApic
		{
			uint8_t Type;
			uint8_t Length;
			uint8_t ApicID;
			uint8_t Reserved;
			uint32_t InterruptBase;
			uint64_t Address;
		} PACKED;

		/** Local SAPIC entry in the MADT. */
		struct LSApic
		{
			uint8_t Type;
			uint8_t Length;
			uint8_t ProcessorID;
			uint8_t LSApicID;
			uint8_t LSApicEID;
			uint8_t Reserved[3];
			uint32_t Flags;
			uint32_t UidValue;
			uint8_t UidString[];
		} PACKED;

		/** Platform interrupt source entry in the MADT. */
		struct PlatformInterrupt
		{
			uint8_t Type;
			uint8_t Length;
			uint16_t Flags;
			uint8_t IrqType;
			uint8_t ProcessorID;
			uint8_t ProcessorEID;
			uint8_t Vector;
			uint32_t Interrupt;
			uint32_t PMIFlags;
		} PACKED;

		/** Local x2APIC entry in the MADT. */
		struct X2Apic
		{
			uint8_t Type;
			uint8_t Length;
			uint16_t Reserved;
			uint32_t X2ApicID;
			uint32_t Flags;
			uint32_t AcpiID;
		} PACKED;

		/** Local x2APIC non-maskable interrupt entry in the MADT. */
		struct X2ApicNMI
		{
			uint8_t Type;
			uint8_t Length;
			uint16_t Flags;
			uint32_t AcpiID;
			uint8_t LINT;
			uint8_t Reserved[3];
		} PACKED;

		/** Smart battery specification table. */
		struct SbsTable
		{
			TableHeader Header;
			uint32_t Warning;
			uint32_t Low;
			uint32_t Critical;
		} PACKED;

		/** Embedded controller boot resources table. */
		struct EcdTable
		{
			TableHeader Header;
			ExtendedAddress Control;
			ExtendedAddress Data;
			uint32_t UID;
			uint8_t GPE;
			uint8_t ID[];
		} PACKED;

		/** Local APIC / SAPIC affinity structure in the SRAT. */
		struct ApicAffinity
		{
			uint8_t Type;
			uint8_t Length;
			uint8_t ProximityLow;
			uint8_t ApicID;
			uint32_t Flags;
			uint8_t EID;
			uint32_t ProximityHigh:24;
			uint32_t ClockDomain;
		} PACKED;

		/** Memory affinity structure in the SRAT. */
		struct MemoryAffinity
		{
			uint8_t Type;
			uint8_t Length;
			uint32_t Proximity;
			uint16_t Reserved1;
			uint64_t MemAddress;
			uint64_t MemLength;
			uint32_t Reserved2;
			uint32_t Flags;
			uint64_t Reserved3;
		} PACKED;

		/** Local x2APIC affinity structure in the SRAT. */
		struct X2ApicAffinity
		{
			uint8_t Type;
			uint8_t Length;
			uint16_t Reserved1;
			uint32_t Proximity;
			uint32_t ApicID;
			uint32_t Flags;
			uint32_t Clock;
			uint32_t Reserved2;
		} PACKED;

		/** System locality distance information table. */
		struct SliTable
		{
			TableHeader Header;
			uint64_t Count;
			uint8_t Entry[];
		} PACKED;

		/** Corrected platform error polling processor structure in the CPEP. */
		struct ErrorPolling
		{
			uint8_t Type;
			uint8_t Length;
			uint8_t ProcessorID;
			uint8_t ProcessorEID;
			uint32_t Interval;
		} PACKED;

		/** Maximum system characteristics table. */
		struct MscTable
		{
			TableHeader Header;
			uint32_t Offset;
			uint32_t ProximityCount;
			uint32_t ClockCount;
			uint64_t MaxPhysAddress;
		} PACKED;

		/** Maximum proximity domain information structure in the MSCT. */
		struct MaxProximity
		{
			uint8_t Revision;
			uint8_t Length;
			uint32_t RangeLow;
			uint32_t RangeHigh;
			uint32_t ProcessorCap;
			uint64_t MemoryCap;
		} PACKED;

		/** High precision event timer table. */
		struct HpetTable
		{
			TableHeader Header;
			union
			{
				uint32_t Value;
				struct
				{
					uint8_t HardwareRevID;
					uint32_t ComparatorCount:5;
					uint32_t CountSize64Bit:1;
					uint32_t Reserved:1;
					uint32_t LegRepIRQRoutCap:1;
					uint16_t VendorID;
				} Data PACKED;
			} EventTimerBlockID;
			ExtendedAddress BaseAddress;
			uint8_t Number;
			uint16_t MinClockTick;
			uint8_t PageProtection;
		} PACKED;

		void ParseFacs(Facs* facs);
		void ParseMadt(MadTable* madt);
		void ParseFadt(FadTable* fadt);
		void ParseHpet(HpetTable* hpet);
		void ParseTable(unsigned long phys);

		void Init(TableHeader* header, unsigned int rev);

		// MADT entry types.
		static const uint8_t ENTRY_LAPIC      = 0x00;
		static const uint8_t ENTRY_IOAPIC     = 0x01;
		static const uint8_t ENTRY_IRQ_SRC    = 0x02;
		static const uint8_t ENTRY_NMI        = 0x03;
		static const uint8_t ENTRY_LAPIC_NMI  = 0x04;
		static const uint8_t ENTRY_LAPIC_ADDR = 0x05;
		static const uint8_t ENTRY_IOSAPIC    = 0x06;
		static const uint8_t ENTRY_LSAPIC     = 0x07;
		static const uint8_t ENTRY_PLATFORM   = 0x08;
		static const uint8_t ENTRY_X2APIC     = 0x09;
		static const uint8_t ENTRY_X2APIC_NMI = 0x0a;

		static const uint8_t CPU_ENABLED      = 0x01;

		bool SearchPointer(Memory::PhysAddr first, Memory::PhysAddr last);

		extern int numcpus, numioapics;
		extern LApic** cpus;
		extern IOApic** ioapics;

		inline int GetProcessorCount(void)
		{
			return numcpus;
		}

		inline LApic* GetProcessor(int n)
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

		int GetISAIRQ(int n);
	}
}
#endif

#endif
