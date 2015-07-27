// DriverThread.cpp - Starts a new driver.

#include <DriverThread.h>
// #include <KThread.h>
#include <PhysicalMemory.h>
#include <StdDef.h>
#include <Console.h>
#include <TaskScheduler.h>

using namespace Kernel;

DriverThread::DriverThread(void)
{
	elf = nullptr;
	owner = nullptr;
	cmd = nullptr;
}

DriverThread::DriverThread(ElfHeader* eh, char* command)
{
	elf = eh;
//	owner = kthread().GetOwner();
	cmd = command;
}

int DriverThread::Main(void)
{
	unsigned long j, k;
	ElfProgramHeader* ph;
	ElfSectionHeader* sh;
	unsigned long bs = physmem().GetBlockSize();
	unsigned long base, length;
	void* entry = (void*)elf->Entry;

	ph = (ElfProgramHeader*)((unsigned long)elf + elf->PHOffset);
	sh = (ElfSectionHeader*)((unsigned long)elf + elf->SHOffset);

//	console().WriteFormat("ph = 0x%16x, sh = 0x%16x, entry = 0x%16x\n", elf->PHOffset, elf->SHOffset, (unsigned long)entry);

//	owner = new Process;
//	tasksched().CreateProcess(owner);

	for(j = 0; j < elf->PHNum; j++)
	{
//		console().WriteFormat("Virt = 0x%16x, Phys = 0x%16x\n, Size = 0x%16x\n", ph[j].VirtAddress, ph[j].PhysAddress, ph[j].MemSize);
//		console().WriteFormat("Type = 0x%16x, Offs = 0x%16x\n, File = 0x%16x\n", ph[j].Type, ph[j].Offset, ph[j].FileSize);

		if(ph[j].Type != PT_LOAD)
			continue;

		base = (ph[j].VirtAddress / bs) * bs;
		length = (ph[j].MemSize + (ph[j].VirtAddress - base) - 1) / bs + 1;

		for(k = 0; k < length; k++)
		{
			if(!physmem().VirtExists((void*)(base + k * bs)))
				physmem().AllocBlocks((void*)(base + k * bs), 1);
		}

		for(k = 0; k < ph[j].FileSize; k++)
			((unsigned char*)(ph[j].VirtAddress))[k] = ((unsigned char*)((unsigned long)elf + ph[j].Offset))[k];

		for(; k < ph[j].MemSize; k++)
			((unsigned char*)(ph[j].VirtAddress))[k] = 0;
	}

	(*(void(*)(char*))entry)(cmd);

	return 0;
}
