// Core.cpp - Core kernel implementation

#include <cstdint>
#include <new>
#include <Core.h>
#include <Symbol.h>
#include <Pager.h>
#include <Console.h>
#include INC_VENDOR(Multiboot.h)
#include INC_ARCH(CPU.h)
#include INC_ARCH(DescriptorTable.h)
#include INC_ARCH(ControlRegisters.h)
#include INC_SUBARCH(Entry.h)
#include INC_SUBARCH(PMode.h)
#include INC_VENDOR(Cmos.h)
#include INC_VENDOR(PIT.h)
#include INC_VENDOR(ACPI.h)
#include INC_VENDOR(SMP.h)
#include INC_VENDOR(PICManager.h)
#include INC_VENDOR(IOApicManager.h)

using namespace Kernel;

extern "C" unsigned long bspStack;

extern "C" void SECTION(".init.text") KernelEntry(uint32_t magic, uint32_t mbiphys)
{
	unsigned int ncpu = 1, bspid = 0;

	// Init console and show message.
	Core::Welcome();

	// Check multiboot magic.
	Multiboot::CheckMagic(magic);

	// Get basic CPU info.
	new (bspcpu_space) CPU;

	// Init descriptor tables.
	new (&tabIDT) DescriptorTable;
	tabIDT.InitIDT();
	new (&tabGDT) DescriptorTable;

	// TODO: Create kernel context object to store pointer to "master" page table.

	// Init memory manager.
	Multiboot::Info* mbi = Multiboot::InitMemory(mbiphys);

	// Init further hardware components.
	Cmos::Init();
	PIT::Init();

#ifdef ELF32
	if(bspcpu().HasLongMode())
		Console::WriteMessage(Console::Style::WARNING, "64 Bit CPU detected, OS level:", "32 Bit");
#endif

	unsigned long cr4 = CR4::Read();
	if(bspcpu().HasPGE())
		cr4 |= CR4::PAGE_GLOBAL_ENABLE;
	if(bspcpu().HasFXSR())
		cr4 |= CR4::OS_FXSR_SUPPORT;
	if(bspcpu().HasXSR())
	{
		cr4 |= CR4::OS_XSR_SUPPORT;
		asm volatile ( "xsetbv" : : "a" (0x7), "d" (0x0), "c" (0x0) );
	}
	CR4::Write(cr4);

	// Figure out where to look for SMP / ACPI information.
#if defined CONFIG_SMP || defined CONFIG_ACPI
	uint16_t segaddr = *(uint16_t*)(0x0000040e + Symbol::kernelOffset.Addr()); // EBDA
	if(!segaddr)
		segaddr = ((*(uint16_t*)(0x00000413 + Symbol::kernelOffset.Addr())) - 1) << 6; // last k of lower memory
	uint32_t physaddr = ((uint32_t)segaddr) << 4;
#endif

#ifdef CONFIG_ACPI
	if((ACPI::SearchPointer(physaddr, physaddr + 0x400) || ACPI::SearchPointer(0x000e0000, 0x00100000)) && ACPI::GetProcessorCount() > 0)
	{
		// TODO: Perform IRQ, tasker, clock initialization from ACPI.

		ncpu = ACPI::GetProcessorCount();
		IOApicManager::InitAcpi();
	}
	else
#endif
#ifdef CONFIG_SMP
	if(SMP::SearchPointer(physaddr, physaddr + 0x400) || SMP::SearchPointer(0x000f0000, 0x00100000))
	{
		// TODO: Perform IRQ, tasker, clock initialization from MP tables.

		ncpu = SMP::GetProcessorCount();
		IOApicManager::InitSmp();

		for(unsigned int i = 0; i < ncpu; i++)
		{
			SMP::Cpu* sc = SMP::GetProcessor(i);
			if(sc->Flags & SMP::CPU_BSP)
				bspid = i;
		}
	}
	else
#endif
	{
		// TODO: Perform IRQ, tasker, clock initialization in single CPU mode.
	}

	for(unsigned int i = 0; i < ncpu; i++)
	{
		TSS* tss = (TSS*)Memory::AllocBlock<Memory::PGB_4K>(TSS_LIN_ADDR + i * TSS_LENGTH, Memory::MemType::KERNEL_RW);
		tss->iobase = 0x1000;
		tabGDT.CreateTSS(FIRST_TSS + i, tss, 0x1000);
	}

	Pager::Map((uintptr_t)&bspStack - Symbol::kernelOffset.Addr() - STACK_SIZE, STACK_LIN_ADDR + bspid * STACK_SIZE, STACK_SIZE, Memory::MemType::KERNEL_RW);
#ifdef ELF32
	asm volatile("addl %0, %%esp" : : "r"(STACK_LIN_ADDR + (bspid + 1) * STACK_SIZE - (uintptr_t)&bspStack));
	SetTaskReg((FIRST_TSS + bspid) << 3);
#else
	asm volatile("addq %0, %%rsp" : : "r"(STACK_LIN_ADDR + (bspid + 1) * STACK_SIZE - (uintptr_t)&bspStack));
	SetTaskReg((FIRST_TSS + bspid) << 4);
#endif

	// Start multiboot modules.
	mbi->InitModules();
}

extern "C" void FreeMemory(void)
{
	Pager::FreeBootMemory();
}

#if defined CONFIG_SMP || defined CONFIG_ACPI
extern "C" void SECTION(".init.text") ApplicationEntry()
{
	// TODO: Check and enable CPU dependent features.

	// TODO: Register CPU as available for execution in tasker.

	// TODO: Set flag to indicate CPU is booted.
}
#endif
