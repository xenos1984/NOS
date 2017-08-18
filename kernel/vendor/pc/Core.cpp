// Core.cpp - Core kernel implementation

#include <cstdint>
#include <new>
#include <Core.h>
#include <Symbol.h>
#include INC_ARCH(Multiboot.h)
#include INC_ARCH(CPU.h)
#include INC_ARCH(DescriptorTable.h)
#include INC_ARCH(Cmos.h)
#include INC_ARCH(PIT.h)
#include INC_ARCH(ACPI.h)
#include INC_ARCH(SMP.h)

using namespace Kernel;

extern "C" void SECTION(".init.text") KernelEntry(uint32_t magic, uint32_t mbiphys)
{
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
	// TODO: Move Cmos and PIT from arch to vendor.
	new (cmos_space) Cmos;
	new (pit_space) PIT;

	// TODO: Check and enable CPU dependent features.

	// Figure out where to look for SMP / ACPI information.
#if defined CONFIG_SMP || defined CONFIG_ACPI
	uint16_t segaddr = *(uint16_t*)(0x0000040e + Symbol::kernelOffset.Addr()); // EBDA
	if(!segaddr)
		segaddr = ((*(uint16_t*)(0x00000413 + Symbol::kernelOffset.Addr())) - 1) << 6; // last k of lower memory
	uint32_t physaddr = ((uint32_t)segaddr) << 4;
#endif

	// TODO: Move ACPI and SMP from arch to vendor.
#ifdef CONFIG_ACPI
	if(ACPI::SearchPointer(physaddr, physaddr + 0x400) || ACPI::SearchPointer(0x000e0000, 0x00100000))
	{
		// TODO: Perform IRQ, tasker, clock initialization from ACPI.
	}
	else
#endif
#ifdef CONFIG_SMP
	if(SMP::SearchPointer(physaddr, physaddr + 0x400) || SMP::SearchPointer(0x000f0000, 0x00100000))
	{
		// TODO: Perform IRQ, tasker, clock initialization from MP tables.
	}
	else
#endif
	{
		// TODO: Perform IRQ, tasker, clock initialization in single CPU mode.
	}

	// Start multiboot modules.
	mbi->InitModules();
}

