// Core.cpp - Core kernel implementation

#include <new>
#include <Core.h>
#include <VirtualMemory.h>
#include <TaskScheduler.h>
#include <Symbol.h>
#include <Memory.h>
#include <Chunker.h>
#include <Process.h>
#include <Console.h>
#include INC_ARCH(X86Pager.h)
#include INC_SUBARCH(Entry.h)
#include INC_ARCH(DescriptorTable.h)
#include INC_ARCH(Cmos.h)
#include INC_ARCH(PIT.h)
#include INC_ARCH(PITClock.h)
#include INC_ARCH(Multiboot.h)
#include INC_ARCH(ACPI.h)
#include INC_ARCH(SMP.h)
#include INC_SUBARCH(I386TaskManager.h)
#include INC_SUBARCH(PMode.h)
#include INC_ARCH(ControlRegisters.h)
#include INC_ARCH(Apic.h)
#include INC_ARCH(CPU.h)

using namespace Kernel;

extern "C" void SECTION(".init.text") KernelEntry(unsigned long magic, Multiboot::Info* mbi)
{
	// Init console and show message.
	Core::Welcome();

	// Get basic CPU info.
	new (bspcpu_space) CPU;

	// Init descriptor tables.
	new (&tabIDT) DescriptorTable;
	tabIDT.InitIDT();
	new (&tabGDT) DescriptorTable;

	// Fill kernel process structure with elementary data.
	new (kprocess_space) Process;
	kprocess().data.cr3 = CR3::Read();
	kprocess().data.pgtab = (X86Pager::PageTable*)tabPGDIR;

	// Init physical memory manager.
	uint32_t mem = ((Multiboot::Info*)(Symbol::kernelOffset.Addr() + (uintptr_t)mbi))->UpperMemory;
	Chunker::Init(1UL << 20, (mem > (15UL << 10) ? 15UL << 20 : mem << 10), Memory::Zone::DMA24);

	new (physmem_space) X86Pager(mbi);
	new (virtmem_space) VirtualMemory();
	new (multiboot_space) Multiboot(magic, mbi);
	new (cmos_space) Cmos;
	new (pit_space) PIT;

	if(bspcpu().HasLongMode())
		Console::WriteMessage(Console::Style::WARNING, "64 Bit CPU detected, OS level:", "32 Bit");

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

#if defined CONFIG_SMP || defined CONFIG_ACPI
	uint16_t segaddr = *(uint16_t*)(0x0000040e + Symbol::kernelOffset.Addr()); // EBDA
	if(!segaddr)
		segaddr = ((*(uint16_t*)(0x00000413 + Symbol::kernelOffset.Addr())) - 1) << 6; // last k of lower memory
	uint32_t physaddr = ((uint32_t)segaddr) << 4;
#endif

#ifdef CONFIG_ACPI
	if(ACPI::Init(physaddr, physaddr + 0x400) || ACPI::Init(0x000e0000, 0x00100000))
	{
		I386TaskManager::InitAcpi();
		new (sysclock_space) PITClock(cmos().GetTime(), acpi().GetISAIRQ(0));
	}
	else
#endif
#ifdef CONFIG_SMP
	if(SMP::Init(physaddr, physaddr + 0x400) || SMP::Init(0x000f0000, 0x00100000))
	{
		I386TaskManager::InitSmp();
		new (sysclock_space) PITClock(cmos().GetTime(), smp().GetISAIRQ(0));
	}
	else
#endif
	{
		I386TaskManager::Init();
		new (sysclock_space) PITClock(cmos().GetTime(), 0);
	}

	multiboot().StartModules();
}

extern "C" void FreeMemory(void)
{
	physmem().CleanInit();
}

#if defined CONFIG_SMP || defined CONFIG_ACPI
bool apflag;

extern "C" void SECTION(".init.text") ApplicationEntry(void)
{
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

	Apic::Enable();
	i386taskman().SetTSS();
	Apic::SetTimerVector(0x40);
	Apic::SetTimerDiv(Apic::TDR_64);
	Apic::TimerStart(0x10000);
	apflag = true;
}
#endif
