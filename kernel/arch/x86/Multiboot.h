// Multiboot.h - Multiboot support for the kernel.

#ifndef __ARCH_X86_MULTIBOOT_H__
#define __ARCH_X86_MULTIBOOT_H__

#include <cstdint>

namespace Kernel
{
	class Multiboot
	{
	public:
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
		struct Module
		{
			uint32_t ModStart;
			uint32_t ModEnd;
			uint32_t String;
			uint32_t Reserved;
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
		struct Info
		{
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
		} PACKED;

	private:
		Info* mbi;
		Module* mbm;

	public:
		static const uint32_t MULTIBOOT_MAGIC = 0x2BADB002; /**< Passed by multiboot loader. */

		static const uint32_t MB_FLAGS_MEMORY_INFO  = 0x00000001;
		static const uint32_t MB_FLAGS_BOOT_DEVICE  = 0x00000002;
		static const uint32_t MB_FLAGS_CMDLINE      = 0x00000004;
		static const uint32_t MB_FLAGS_MODULES      = 0x00000008;
		static const uint32_t MB_FLAGS_SYMBOLS_AOUT = 0x00000010;
		static const uint32_t MB_FLAGS_SYMBOLS_ELF  = 0x00000020;
		static const uint32_t MB_FLAGS_MEMORY_MAP   = 0x00000040;
		static const uint32_t MB_FLAGS_DRIVES       = 0x00000080;
		static const uint32_t MB_FLAGS_CONFIG       = 0x00000100;
		static const uint32_t MB_FLAGS_NAME         = 0x00000200;
		static const uint32_t MB_FLAGS_APM_TAB      = 0x00000400;
		static const uint32_t MB_FLAGS_VBE_INFO     = 0x00000800;

		Multiboot(unsigned long magic, Info* mbi);

		inline Info* GetInfo(void)
		{
			return mbi;
		}

		void StartModules(void);
	};
}

extern char multiboot_space[];
inline Kernel::Multiboot& multiboot(void) { return(reinterpret_cast<Kernel::Multiboot&>(multiboot_space)); }

#endif
