// Multiboot.cpp - Multiboot support for the kernel.

#include <Elf.h>
#include <Console.h>
#include INC_ARCH(X86Pager.h)
#include INC_ARCH(Multiboot.h)
#include <TaskManager.h>
#include <VirtualMemory.h>

using namespace Kernel;

char multiboot_space[sizeof(Multiboot)];

SECTION(".init.text") Multiboot::Multiboot(unsigned long magic, Info* info)
{
	MemoryMap* mmap;
	MemoryMap* mmap0;
	Drive* drive;
	Drive* drive0;
	unsigned int i;

	mbi = (Info*)x86pager().MapTempRegion((unsigned long)info, sizeof(Info));

	// Print some multiboot info.
	console().WriteMessage((magic == MULTIBOOT_MAGIC ? Console::MSG_OK : Console::MSG_WARNING), "Multiboot Info:", "Magic: 0x%8x, Address: 0x%8x, Flags: 0x%8x", magic, info, mbi->Flags);

	if(mbi->Flags & MB_FLAGS_MEMORY_MAP)
	{
		console().WriteFormat("Memory map of length 0x%8x at address 0x%8x\n", mbi->MemoryMapLength, mbi->MemoryMapAddress);
		mmap0 = (MemoryMap*)x86pager().MapTempRegion(mbi->MemoryMapAddress, mbi->MemoryMapLength);
		for(mmap = mmap0; (unsigned long)mmap - (unsigned long)mmap0 < mbi->MemoryMapLength; mmap = (MemoryMap*)((unsigned long)mmap + mmap->Size + 4))
		{
			console().WriteFormat("Mem: 0x%16lx-0x%16lx, Type: 0x%2x\n", mmap->BaseAddr, mmap->BaseAddr + mmap->Length - 1, mmap->Type);
			x86pager().AddMemoryArea(mmap);
		}
	}

	if(mbi->Flags & MB_FLAGS_MEMORY_INFO)
	{
		console().WriteMessage(Console::MSG_INFO, "Base contiguous memory:", "%u kB", mbi->UpperMemory + 1024);
	}

	if(mbi->Flags & MB_FLAGS_DRIVES)
	{
		console().WriteFormat("Drive list of length 0x%8x at address 0x%8x\n", mbi->DrivesLength, mbi->DrivesAddress);
		drive0 = (Drive*)x86pager().MapTempRegion(mbi->DrivesAddress, mbi->DrivesLength);
		for(drive = drive0; (unsigned long)drive - (unsigned long)drive0 < mbi->DrivesLength; drive = (Drive*)((unsigned long)drive + drive->Size))
			console().WriteFormat("Number: 0x%2x, Mode: %s, C: %d, H: %d, S: %d\n", drive->Number, (drive->Mode ? "LBA" : "CHS"), drive->Cylinders, drive->Heads, drive->Sectors);
	}

	if(mbi->Flags & MB_FLAGS_BOOT_DEVICE)
		console().WriteMessage(Console::MSG_INFO, "Boot device:", "Drive: 0x%2x, Partition: %d / %d / %d", mbi->BootDrive, mbi->BootPartition1, mbi->BootPartition2, mbi->BootPartition3);

	if(mbi->Flags & MB_FLAGS_NAME)
		console().WriteMessage(Console::MSG_INFO, "Bootloader @ 0x%8x:", "%s", mbi->BootLoaderName, x86pager().MapTempString(mbi->BootLoaderName));

	if(mbi->Flags & MB_FLAGS_CMDLINE)
		console().WriteMessage(Console::MSG_INFO, "Command line @ 0x%8x:", "%s", mbi->CommandLine, x86pager().MapTempString(mbi->CommandLine));

	if(mbi->Flags & MB_FLAGS_MODULES)
	{
		console().WriteMessage(Console::MSG_INFO, "Boot modules:", "@ 0x%8x", mbi->ModuleAddress);
		mbm = (Module*)x86pager().MapTempRegion(mbi->ModuleAddress, mbi->ModuleCount * sizeof(Module));
		for(i = 0; i < mbi->ModuleCount; i++)
			console().WriteMessage(Console::MSG_INFO, "Boot module:", "0x%8x - 0x%8x", mbm[i].ModStart, mbm[i].ModEnd);
	}
}

void SECTION(".init.text") Multiboot::StartModules(void)
{
	unsigned int i;
	Elf::Header* module;
	Process* p;

	if(!(mbi->Flags & MB_FLAGS_MODULES))
		return;

	for(i = 0; i < mbi->ModuleCount; i++)
	{
		module = (Elf::Header*)x86pager().MapTempRegion(mbm[i].ModStart, mbm[i].ModEnd - mbm[i].ModStart + 1);

		if(module->Magic[0] != Elf::ELF_MAG0 || module->Magic[1] != Elf::ELF_MAG1 || module->Magic[2] != Elf::ELF_MAG2 || module->Magic[3] != Elf::ELF_MAG3)
		{
			console().WriteMessage(Console::MSG_WARNING, "Module at 0x%8x:", "Invalid ELF.", mbm[i].ModStart);
			continue;
		}
#if defined ELF32
		if(module->Class != Elf::ELF_CLASS_32)
		{
			console().WriteMessage(Console::MSG_WARNING, "Module at 0x%8x:", "No 32bit module.", mbm[i].ModStart);
			continue;
		}
#elif defined ELF64
		if(module->Class != Elf::ELF_CLASS_64)
		{
			console().WriteMessage(Console::MSG_WARNING, "Module at 0x%8x:", "No 64bit module.", mbm[i].ModStart);
			continue;
		}
#endif
		if(module->Machine != kernelElfHeader.Machine)
		{
			console().WriteMessage(Console::MSG_WARNING, "Module at 0x%8x:", "Incorrect architecture 0x%4x, expected 0x%4x.", mbm[i].ModStart, module->Machine, kernelElfHeader.Machine);
			continue;
		}

		if(!module->PHNum)
		{
			console().WriteMessage(Console::MSG_WARNING, "Module at 0x%8x:", "No program header found.", mbm[i].ModStart);
			continue;
		}

		if(!module->SHNum)
		{
			console().WriteMessage(Console::MSG_WARNING, "Module at 0x%8x:", "No section header found.", mbm[i].ModStart);
			continue;
		}

		if(module->Type == Elf::ET_EXEC)
		{
			p = taskman().CreateProcess((Elf*)module);
			console().WriteMessage(Console::MSG_OK, "Executable Module at 0x%8x:", "Started with %d bytes.", mbm[i].ModStart, p->memory.Value());
			continue;
		}

		if(module->Type == Elf::ET_DYN)
		{
			console().WriteMessage(Console::MSG_WARNING, "Library Module at 0x%8x:", "Not implemented.", mbm[i].ModStart);
			continue;
		}

		console().WriteMessage(Console::MSG_WARNING, "Module at 0x%8x:", "Unknown type 0x%4x, unable to execute.", mbm[i].ModStart, module->Type);
	}
}
