// Core.cpp - Core kernel implementation

#include <cstdint>
#include <new>
#include <Core.h>
#include <Clock.h>
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
#include INC_SUBARCH(X86_64TaskManager.h)
#include INC_SUBARCH(PMode.h)
#include INC_ARCH(Apic.h)
#include INC_ARCH(ControlRegisters.h)
#include INC_ARCH(MSR.h)
#include INC_ARCH(Processor.h)
#include INC_ARCH(CPU.h)
#include INC_ARCH(PICManager.h)
#include INC_ARCH(IOApicManager.h)

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

	// Fill kernel process structure with elementary data.
	new (kprocess_space) Process;
	kprocess().data.cr3 = CR3::Read();
	kprocess().data.pgtab = (X86Pager::PageTable*)tabPML4T;

	// Init memory manager.
	Multiboot::Info* mbi = Multiboot::InitMemory(mbiphys);

	new (cmos_space) Cmos;
	new (pit_space) PIT;
	new (irqman_space) PICManager();

	unsigned long cr4 = CR4::Read();
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
	if(ACPI::SearchPointer(physaddr, physaddr + 0x400) || ACPI::SearchPointer(0x000e0000, 0x00100000))
	{
		IOApicManager::InitAcpi();
		X86_64TaskManager::InitAcpi();
		new (sysclock_space) PITClock(cmos().GetTime(), ACPI::GetISAIRQ(0));
	}
	else
#endif
#ifdef CONFIG_SMP
	if(SMP::Init(physaddr, physaddr + 0x400) || SMP::Init(0x000f0000, 0x00100000))
	{
		IOApicManager::InitSmp();
		X86_64TaskManager::InitSmp();
		new (sysclock_space) PITClock(cmos().GetTime(), smp().GetISAIRQ(0));
	}
	else
#endif
	{
		X86_64TaskManager::Init();
		new (sysclock_space) PITClock(cmos().GetTime(), 0);
	}
	asm volatile ("hlt");

	mbi->InitModules();
}

extern "C" void FreeMemory(void)
{
	physmem().CleanInit();
}

#if defined CONFIG_SMP || defined CONFIG_ACPI
bool apflag;

extern "C" void SECTION(".init.text") ApplicationEntry()
{
	unsigned long cr4 = CR4::Read();
	cr4 |= CR4::OS_FXSR_SUPPORT;
	if(bspcpu().HasXSR())
	{
		cr4 |= CR4::OS_XSR_SUPPORT;
		asm volatile ( "xsetbv" : : "a" (0x7), "d" (0x0), "c" (0x0) );
	}
	CR4::Write(cr4);

	Apic::Enable();
	x86_64taskman().SetTSS();
	MSR::Write(MSR::KERNEL_GS_BASE, TSS_LIN_ADDR + taskman().GetCurrentCPU() * TSS_LENGTH);
	Apic::SetTimerVector(0x40);
	Apic::SetTimerDiv(Apic::TDR_64);
	Apic::TimerStart(0x10000);
	apflag = true;
}
#endif
