// Multiboot.cpp - Multiboot support for the kernel.

#include <Elf.h>
#include <Console.h>
#include <Symbol.h>
#include INC_VENDOR(Multiboot.h)
#include <TaskManager.h>
#include <Heap.h>
#include <Chunker.h>
#include <Pager.h>
#include <new>

namespace Kernel
{
	namespace Multiboot
	{
		SECTION(".init.text") bool CheckMagic(uint32_t magic)
		{
			Console::WriteMessage((magic == MAGIC ? Console::Style::OK : Console::Style::WARNING), "Multiboot magic:", "0x%8x", magic);
			return (magic == MAGIC);
		}

		SECTION(".init.text") Info* InitMemory(Memory::PhysAddr mbiphys)
		{
			Info* mbi;
			MemoryMap* mmap;
			MemoryMap* mmap0;
			Drive* drive;
			Drive* drive0;
			Memory::Zone zone;

			// Map multiboot info.
			mbi = reinterpret_cast<Info*>(Pager::MapBootRegion(mbiphys, sizeof(Info), Memory::MemType::KERNEL_RO));
			Console::WriteFormat("Multiboot info mapped from 0x%8x to 0x%p.\n", mbiphys, mbi);

			// Check for memory hole below 16MB.
			uint32_t mem = mbi->UpperMemory;
			uint32_t length = (mem > (15UL << 10) ? 15UL << 20 : mem << 10);
			Console::WriteFormat("Upper memory: %dkB\n", mem);

			// Give memory between 1MB and 16MB (or start of hole) to the chunker.
			Chunker::Init(1UL << 20, length, Memory::Zone::DMA24);

			// Mark space occupied by the kernel as used.
			Chunker::Reserve(1UL << 20, Symbol::kernelEnd.Addr() - Symbol::kernelOffset.Addr());

			// Mark occupied if there is any multiboot information or modules in the area given to the chunker.
			mbi->MarkReserved();

			// Initialize heap.
			Heap::Init();

			// If there is a memory map, map that as well.
			if(mbi->HasMemMap())
			{
				Console::WriteFormat("Memory map of length 0x%8x at address 0x%8x\n", mbi->MemoryMapLength, mbi->MemoryMapAddress);
				mmap0 = reinterpret_cast<MemoryMap*>(Pager::MapBootRegion(mbi->MemoryMapAddress, mbi->MemoryMapLength, Memory::MemType::KERNEL_RO));
				for(mmap = mmap0; (uintptr_t)mmap - (uintptr_t)mmap0 < mbi->MemoryMapLength; mmap = (MemoryMap*)((uintptr_t)mmap + mmap->Size + 4))
				{
					Console::WriteFormat("Mem: 0x%16lx-0x%16lx, Type: 0x%2x\n", mmap->BaseAddr, mmap->BaseAddr + mmap->Length - 1, mmap->Type);
					if(mmap->Type == 1)
					{
						if(mmap->BaseAddr < 0x00100000ULL)
							zone = Memory::Zone::REAL;
						else if(mmap->BaseAddr < 0x01000000ULL)
							zone = Memory::Zone::DMA24;
						else if(mmap->BaseAddr < 0x100000000ULL)
							zone = Memory::Zone::DMA32;
						else
							zone = Memory::Zone::HIGH;
						if(sizeof(Memory::PhysAddr) == 8 || zone != Memory::Zone::HIGH)
						{
							Chunker::AddRegion(mmap->BaseAddr, mmap->Length, zone);
							mbi->MarkReserved();
						}
					}
				}
			}

			if(mbi->HasMemInfo())
			{
				Console::WriteMessage(Console::Style::INFO, "Base contiguous memory:", "%u kB", mbi->UpperMemory + 1024);
			}

			if(mbi->HasDrives())
			{
				Console::WriteFormat("Drive list of length 0x%8x at address 0x%8x\n", mbi->DrivesLength, mbi->DrivesAddress);
				drive0 = (Drive*)Pager::MapBootRegion(mbi->DrivesAddress, mbi->DrivesLength, Memory::MemType::KERNEL_RO);
				for(drive = drive0; (unsigned long)drive - (unsigned long)drive0 < mbi->DrivesLength; drive = (Drive*)((unsigned long)drive + drive->Size))
					Console::WriteFormat("Number: 0x%2x, Mode: %s, C: %d, H: %d, S: %d\n", drive->Number, (drive->Mode ? "LBA" : "CHS"), drive->Cylinders, drive->Heads, drive->Sectors);
			}

			if(mbi->HasBootDev())
				Console::WriteMessage(Console::Style::INFO, "Boot device:", "Drive: 0x%2x, Partition: %d / %d / %d", mbi->BootDrive, mbi->BootPartition1, mbi->BootPartition2, mbi->BootPartition3);

			if(mbi->HasLoaderName())
				Console::WriteMessage(Console::Style::INFO, "Bootloader @ 0x%8x:", "%s", mbi->BootLoaderName, Pager::MapBootString(mbi->BootLoaderName, Memory::MemType::KERNEL_RO));

			if(mbi->HasCmdline())
				Console::WriteMessage(Console::Style::INFO, "Command line @ 0x%8x:", "%s", mbi->CommandLine, Pager::MapBootString(mbi->CommandLine, Memory::MemType::KERNEL_RO));

			return mbi;
		}

		SECTION(".init.text") void Info::MarkReserved(void)
		{
			if(HasDrives() && (DrivesLength > 0))
				Chunker::Reserve(DrivesAddress, DrivesAddress + DrivesLength);
			if(HasMemMap() && (MemoryMapLength > 0))
				Chunker::Reserve(MemoryMapAddress, MemoryMapAddress + MemoryMapLength);
			if(HasLoaderName())
				Chunker::Reserve(BootLoaderName, BootLoaderName + (1UL << Memory::MinPageBits));
			if(HasCmdline())
				Chunker::Reserve(CommandLine, CommandLine + (1UL << Memory::MinPageBits));
			if(HasModules() && (ModuleCount > 0))
				Chunker::Reserve(ModuleAddress, ModuleAddress + ModuleCount * sizeof(Module));
		}

		SECTION(".init.text") void Info::InitModules(void)
		{
			if((Flags & FLAGS_MODULES) && (ModuleCount > 0))
			{
				Console::WriteMessage(Console::Style::INFO, "Boot modules:", "Found %d modules @ 0x%8x.", ModuleCount, ModuleAddress);
				Module* mbm = (Module*)Pager::MapBootRegion(ModuleAddress, ModuleCount * sizeof(Module), Memory::MemType::KERNEL_RO);
				for(unsigned int i = 0; i < ModuleCount; i++)
				{
					Console::WriteMessage(Console::Style::INFO, "Boot module:", "0x%8x - 0x%8x", mbm[i].ModStart, mbm[i].ModEnd);
					mbm[i].Start();
				}
			}
			else
				Console::WriteMessage(Console::Style::WARNING, "Boot modules:", "not found.");
		}

		SECTION(".init.text") bool Module::Start(void)
		{
			const char* cmd;
			Elf* elf = (Elf*)Pager::MapBootRegion(ModStart, ModEnd - ModStart + 1, Memory::MemType::KERNEL_RO);
			Elf::Header* ehdr = elf->GetHeader();

			if(String)
			{
				cmd = (const char*)Pager::MapBootString(String, Memory::MemType::KERNEL_RO);
				Console::WriteMessage(Console::Style::INFO, "Module at 0x%8x:", "%s", ModStart, cmd);
			}
			else
				cmd = "";

			if(!elf->Check())
			{
				Console::WriteMessage(Console::Style::ERROR, "Module at 0x%8x:", "ELF check failed.", ModStart);
				return false;
			}

			if(ehdr->Type == Elf::ET_EXEC)
			{
				Process* p = taskman().CreateProcess(elf);
				Console::WriteMessage(Console::Style::OK, "Executable Module at 0x%8x:", "Started with %d bytes.", ModStart, p->memory.load(std::memory_order_relaxed));
				return true;
			}

			if(ehdr->Type == Elf::ET_DYN)
			{
				Console::WriteMessage(Console::Style::WARNING, "Library Module at 0x%8x:", "Not implemented.", ModStart);
				return false;
			}

			Console::WriteMessage(Console::Style::WARNING, "Module at 0x%8x:", "Unknown type 0x%4x, unable to execute.", ModStart, ehdr->Type);
			return false;
		}
	}
}
