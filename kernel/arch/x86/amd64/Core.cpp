// Core.cpp - Core kernel implementation

#include <new>
#include <Core.h>
#include <Clock.h>
#include <VirtualMemory.h>
#include <TaskScheduler.h>
#include <Symbol.h>
#include INC_ARCH(X86Process.h)
#include INC_ARCH(VGAConsole.h)
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

extern "C" void SECTION(".init.text") KernelEntry(unsigned long magic, Multiboot::Info* mbi)
{
	new (console_space) VGAConsole;
	Core::Welcome();
	new (bspcpu_space) CPU;
	new (&tabIDT) DescriptorTable;
	tabIDT.InitIDT();
	new (&tabGDT) DescriptorTable;
	new (kprocess_space) Process;
	kprocess().data.cr3 = CR3::Read();
	kprocess().data.pgtab = (X86Pager::PageTable*)tabPML4T;
	new (physmem_space) X86Pager(mbi);
	new (virtmem_space) VirtualMemory();
	new (multiboot_space) Multiboot(magic, mbi);
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
	unsigned short segaddr = *(unsigned short*)(0x0000040e + Symbol::kernelOffset.Addr()); // EBDA
	if(!segaddr)
		segaddr = ((*(unsigned short*)(0x00000413 + Symbol::kernelOffset.Addr())) - 1) << 6; // last k of lower memory
	unsigned long physaddr = ((unsigned long)segaddr) << 4;
#endif

#ifdef CONFIG_ACPI
	if(ACPI::Init(physaddr, physaddr + 0x400) || ACPI::Init(0x000e0000, 0x00100000))
	{
		IOApicManager::InitAcpi();
		X86_64TaskManager::InitAcpi();
		new (sysclock_space) PITClock(cmos().GetTime(), acpi().GetISAIRQ(0));
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

	multiboot().StartModules();
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

	apic().Enable();
	x86_64taskman().SetTSS();
	MSR::Write(MSR::KERNEL_GS_BASE, TSS_LIN_ADDR + taskman().GetCurrentCPU() * TSS_LENGTH);
	apic().SetTimerVector(0x40);
	apic().SetTimerDiv(Apic::TDR_64);
	apic().TimerStart(0x10000);
	apflag = true;
}
#endif
