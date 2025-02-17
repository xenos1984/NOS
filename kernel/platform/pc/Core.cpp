// Core.cpp - Core kernel implementation

#include <cstdint>
#include <new>
#include <Core.h>
#include <Symbol.h>
#include <Chunker.h>
#include <Pager.h>
#include <Heap.h>
#include <Allocator.h>
#include <Processor.h>
#include <Console.h>
#include INC_PLATFORM(Multiboot.h)
#include INC_ARCH(CPU.h)
#include INC_ARCH(Apic.h)
#include INC_ARCH(DescriptorTable.h)
#include INC_ARCH(ControlRegisters.h)
#include INC_BITS(Entry.h)
#include INC_BITS(PMode.h)
#include INC_PLATFORM(Cmos.h)
#include INC_PLATFORM(PIT.h)
#include INC_PLATFORM(PIC.h)
#include INC_PLATFORM(ACPI.h)
#include INC_PLATFORM(SMP.h)
#include INC_PLATFORM(PICManager.h)
#include INC_PLATFORM(IOApicManager.h)

using namespace Kernel;

extern "C" unsigned long bspStack;
extern "C" unsigned long apcount;
extern "C" unsigned char apentry[];

extern "C" void SECTION(".init.text") KernelEntry(uint32_t magic, uint32_t mbiphys)
{
	Processor::nproc = 1;

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
	PIC::Master.SetMaster(0x20);
	PIC::Slave.SetSlave(0x28, &PIC::Master);
	PIC::Master.UnmaskIRQ(2);

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
		cr4 |= CR4::OS_XSR_SUPPORT;
	CR4::Write(cr4);
	if(bspcpu().HasXSR())
		asm volatile ( "xsetbv" : : "a" (0x7), "d" (0x0), "c" (0x0) );

#if defined CONFIG_SMP || defined CONFIG_ACPI
	// Figure out where to look for SMP / ACPI information.
	uint16_t segaddr = *(uint16_t*)(0x0000040e + Symbol::kernelOffset.Addr()); // EBDA
	if(!segaddr)
		segaddr = ((*(uint16_t*)(0x00000413 + Symbol::kernelOffset.Addr())) - 1) << 6; // last k of lower memory
	uint32_t physaddr = ((uint32_t)segaddr) << 4;
#endif

#ifdef CONFIG_ACPI
	if((ACPI::SearchPointer(physaddr, physaddr + 0x400) || ACPI::SearchPointer(0x000e0000, 0x00100000)) && ACPI::GetProcessorCount() > 0)
	{
		// TODO: Perform IRQ, tasker, clock initialization from ACPI.

		Processor::nproc = ACPI::GetProcessorCount();
		Processor::proc = new Processor[Processor::nproc];
		IOApicManager::InitAcpi();

		ACPI::LApic* ac = ACPI::GetProcessor(0);
		Processor::proc[0].state = Processor::State::Online;
		Processor::proc[0].data.physID = ac->ApicID;

		for(unsigned int i = 1; i < Processor::nproc; i++)
		{
			ac = ACPI::GetProcessor(i);
			Processor::proc[i].state = Processor::State::Offline;
			Processor::proc[i].data.physID = ac->ApicID;
		}
	}
	else
#endif
#ifdef CONFIG_SMP
	if(SMP::SearchPointer(physaddr, physaddr + 0x400) || SMP::SearchPointer(0x000f0000, 0x00100000))
	{
		// TODO: Perform IRQ, tasker, clock initialization from MP tables.

		Processor::nproc = SMP::GetProcessorCount();
		Processor::proc = new Processor[Processor::nproc];
		IOApicManager::InitSmp();

		for(unsigned int i = 0; i < Processor::nproc; i++)
		{
			SMP::Cpu* sc = SMP::GetProcessor(i);
			Processor::proc[i].data.physID = sc->LocalApicID;
			Processor::proc[i].state = (sc->Flags & SMP::CPU_BSP ? Processor::State::Online : Processor::State::Offline);
		}
	}
	else
#endif
	{
		// TODO: Perform IRQ, tasker, clock initialization in single CPU mode.

		PICManager::Init();
	}

	Console::WriteMessage(Console::Style::OK, "Processors:", "%u found.", Processor::nproc);

	// Create task state segments.
	for(unsigned int i = 0; i < Processor::nproc; i++)
	{
		TSS* tss = (TSS*)Allocator::AllocBlock<Memory::PGB_4K>(TSS_LIN_ADDR + i * TSS_LENGTH, Memory::MemType::KERNEL_RW);
		tss->iobase = 0x1000;
		tabGDT.CreateTSS(FIRST_TSS + i, tss, 0x1000);
	}

	// Move virtual address of BSP stack.
	Pager::Map((uintptr_t)&bspStack - Symbol::kernelOffset.Addr() - STACK_SIZE, STACK_LIN_ADDR, STACK_SIZE, Memory::MemType::KERNEL_RW);
#ifdef ELF32
	asm volatile("addl %0, %%esp" : : "r"(STACK_LIN_ADDR + STACK_SIZE - (uintptr_t)&bspStack));
	SetTaskReg(FIRST_TSS << 3);
#else
	asm volatile("addq %0, %%rsp" : : "r"(STACK_LIN_ADDR + STACK_SIZE - (uintptr_t)&bspStack));
	SetTaskReg(FIRST_TSS << 4);
#endif

#if defined CONFIG_SMP || defined CONFIG_ACPI
	if(Processor::nproc > 1)
	{
		// Copy AP startup code into lower memory.
		Memory::PhysAddr phys = Chunker::Alloc<Memory::PGB_4K>(Memory::Zone::REAL);
		unsigned int vector = phys >> Memory::PGB_4K;
		unsigned char* apcode = (unsigned char*)(phys + Symbol::kernelOffset.Addr());
		Pager::MapPage<Memory::PGB_4K>(phys, (uintptr_t)apcode, Memory::MemType::KERNEL_RW);
		Pager::MapPage<Memory::PGB_4K>(phys, phys, Memory::MemType::KERNEL_RW);
		Pager::MapPage<Memory::PGB_4K>(((uintptr_t)&tabGDT) - Symbol::kernelOffset.Addr(), ((uintptr_t)&tabGDT) - Symbol::kernelOffset.Addr(), Memory::MemType::KERNEL_RW);
		Pager::MapPage<Memory::PGB_4K>((((uintptr_t)apentry) & ~Memory::PGM_4K) - Symbol::kernelOffset.Addr(), (((uintptr_t)apentry) & ~Memory::PGM_4K) - Symbol::kernelOffset.Addr(), Memory::MemType::KERNEL_RW);
		Console::WriteMessage(Console::Style::INFO, "Core:", "AP startup code copied to %p.", phys);

		for(int i = 0; i < 0x1000; i++)
			apcode[i] = apentry[i];

		// Allocate AP stacks.
		Allocator::Alloc(STACK_LIN_ADDR + STACK_SIZE, (Processor::nproc - 1) * STACK_SIZE, Memory::MemType::KERNEL_RW);

		for(unsigned int i = 0; i < Processor::nproc; i++)
		{
			if(Processor::proc[i].state == Processor::State::Online)
				continue;

			Processor::proc[i].state = Processor::State::Booting;

			Cmos::SetShutdownStatus(Cmos::SH_JMP467);
			*(unsigned int *)(0x00000467 + Symbol::kernelOffset.Addr()) = vector;
			Apic::ClearError();
			Apic::SendIPI(Processor::proc[i].data.physID, 0, Apic::DEST_FIELD | Apic::INT_ASSERT | Apic::DM_INIT); // assert INIT IPI
			while(Apic::SendPending()) ;
			Apic::ClearError();
			Apic::SendIPI(Processor::proc[i].data.physID, 0, Apic::DEST_FIELD | Apic::DM_INIT); // deassert INIT IPI
			while(Apic::SendPending()) ;
			PIT::SetOneShot(0, 11932); // 10ms
			while(PIT::GetCurrent(0) <= 11932) ;

			if(Apic::GetVersion() >= 0x10)
			{
				Apic::ClearError();
				Apic::SendIPI(Processor::proc[i].data.physID, vector, Apic::DEST_FIELD | Apic::DM_STARTUP);
				while(Apic::SendPending()) ;
				PIT::SetOneShot(0, 239); // 200µs
				while(PIT::GetCurrent(0) <= 239) ;
				if(Processor::proc[i].state != Processor::State::Online)
				{
					Apic::ClearError();
					Apic::SendIPI(Processor::proc[i].data.physID, vector, Apic::DEST_FIELD | Apic::DM_STARTUP);
					while(Apic::SendPending()) ;
					PIT::SetOneShot(0, 239); // 200µs
					while(PIT::GetCurrent(0) <= 239) ;
				}
			}

			PIT::SetOneShot(0, 11932); // 10ms
			while((PIT::GetCurrent(0) <= 11932) && (Processor::proc[i].state != Processor::State::Online)) ;
			Cmos::SetShutdownStatus(Cmos::SH_NORMAL);

			if(Processor::proc[i].state == Processor::State::Online)
			{
				Console::WriteMessage(Console::Style::OK, "AP no. 0x%2x: ", "booted", Processor::proc[i].data.physID);
			}
			else
			{
				Console::WriteMessage(Console::Style::WARNING, "AP no. 0x%2x: ", "no response, disabled", Processor::proc[i].data.physID);
				Processor::proc[i].state = Processor::State::Offline;
			}
		}
	}
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

	for(unsigned int i = 0; i < Processor::nproc; i++)
	{
		if(Processor::proc[i].data.physID == Apic::GetPhysID())
		{
			Processor::proc[i].state = Processor::State::Online;
			break;
		}
	}
}
#endif
