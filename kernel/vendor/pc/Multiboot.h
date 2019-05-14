// Multiboot.h - Multiboot support for the kernel.

#ifndef __VENDOR_PC_MULTIBOOT_H__
#define __VENDOR_PC_MULTIBOOT_H__

#include <cstdint>
#include <Memory.h>

namespace Kernel
{
	namespace Multiboot
	{
		static const uint32_t MAGIC = 0x2BADB002; /**< Passed by multiboot loader. */

		enum Flags : uint32_t
		{
			FLAGS_MEMORY_INFO  = 0x00000001,
			FLAGS_BOOT_DEVICE  = 0x00000002,
			FLAGS_CMDLINE      = 0x00000004,
			FLAGS_MODULES      = 0x00000008,
			FLAGS_SYMBOLS_AOUT = 0x00000010,
			FLAGS_SYMBOLS_ELF  = 0x00000020,
			FLAGS_MEMORY_MAP   = 0x00000040,
			FLAGS_DRIVES       = 0x00000080,
			FLAGS_CONFIG       = 0x00000100,
			FLAGS_NAME         = 0x00000200,
			FLAGS_APM_TAB      = 0x00000400,
			FLAGS_VBE_INFO     = 0x00000800
		};

		/** AOut symbols passed in multiboot info structure. */
		struct AOutSymbols
		{
			uint32_t TabSize;
			uint32_t StrSize;
			uint32_t Addr;
			uint32_t Reserved;
		} PACKED;

		/** ELF info passed in multiboot info structure. */
		struct ElfSectionHeader
		{
			uint32_t Num;
			uint32_t Size;
			uint32_t Addr;
			uint32_t Shndx;
		} PACKED;

		/** Module list entry. */
		class Module
		{
		public:
			uint32_t ModStart;
			uint32_t ModEnd;
			uint32_t String;
			uint32_t Reserved;

			Module(void) = delete;
			bool Start(void);
		} PACKED;

		/** Memory map entry. */
		struct MemoryMap
		{
			uint32_t Size;
			uint64_t BaseAddr;
			uint64_t Length;
			uint32_t Type;
		} PACKED;

		/** Drive list entry. */
		struct Drive
		{
			uint32_t Size;
			uint8_t Number;
			uint8_t Mode;
			uint16_t Cylinders;
			uint8_t Heads;
			uint8_t Sectors;
		} PACKED;

		/** Multiboot info structure. */
		class Info
		{
		public:
			uint32_t Flags;
			uint32_t LowerMemory;
			uint32_t UpperMemory;
			uint8_t BootPartition3;
			uint8_t BootPartition2;
			uint8_t BootPartition1;
			uint8_t BootDrive;
			uint32_t CommandLine;
			uint32_t ModuleCount;
			uint32_t ModuleAddress;
			union
			{
				AOutSymbols AoutSyms;
				ElfSectionHeader ElfSects;
			} Symbols;
			uint32_t MemoryMapLength;
			uint32_t MemoryMapAddress;
			uint32_t DrivesLength;
			uint32_t DrivesAddress;
			uint32_t ConfigTable;
			uint32_t BootLoaderName;
			uint32_t ApmTable;
			uint32_t VbeControlInfo;
			uint32_t VbeModeInfo;
			uint16_t VbeMode;
			uint16_t VbeInterfaceSeg;
			uint16_t VbeInterfaceOff;
			uint16_t VbeInterfaceLength;

			Info(void) = delete;

			void InitModules(void);
			void MarkReserved(void);

			inline bool HasMemInfo(void)
			{
				return Flags & FLAGS_MEMORY_INFO;
			}

			inline bool HasBootDev(void)
			{
				return Flags & FLAGS_BOOT_DEVICE;
			}

			inline bool HasCmdline(void)
			{
				return Flags & FLAGS_CMDLINE;
			}

			inline bool HasModules(void)
			{
				return Flags & FLAGS_MODULES;
			}

			inline bool HasSymsAout(void)
			{
				return Flags & FLAGS_SYMBOLS_AOUT;
			}

			inline bool HasSymsElf(void)
			{
				return Flags & FLAGS_SYMBOLS_ELF;
			}

			inline bool HasMemMap(void)
			{
				return Flags & FLAGS_MEMORY_MAP;
			}

			inline bool HasDrives(void)
			{
				return Flags & FLAGS_DRIVES;
			}

			inline bool HasConfig(void)
			{
				return Flags & FLAGS_CONFIG;
			}

			inline bool HasLoaderName(void)
			{
				return Flags & FLAGS_NAME;
			}

			inline bool HasApmTable(void)
			{
				return Flags & FLAGS_APM_TAB;
			}

			inline bool HasVbeInfo(void)
			{
				return Flags & FLAGS_VBE_INFO;
			}
		} PACKED;

		Info* InitMemory(Memory::PhysAddr mbiphys);
		bool CheckMagic(uint32_t magic);
	}
}

#endif
