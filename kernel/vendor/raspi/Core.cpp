// Core.cpp - Core kernel implementation

#include <cstdint>
#include <Core.h>
#include <Console.h>
#include <Symbol.h>
#include <Pager.h>
#include <Chunker.h>
#include <Processor.h>
#include <Heap.h>
#include INC_VENDOR(UART.h)
#include INC_VENDOR(Timer.h)
#include INC_VENDOR(Mailbox.h)
#include INC_BITS(SystemRegs.h)

using namespace Kernel;

extern unsigned int apflag;

extern "C" void SECTION(".init.text") KernelEntry(uint32_t r0, uint32_t r1, uint32_t r2)
{
	Mailbox::MemInfo armmem, vidmem;

	UART::Init();
	Core::Welcome();

#ifdef ELF64
	Sysreg::VBAR_EL1::Write(Symbol::ExceptionVectorBase.Addr());
#else
#ifdef ARM_EXT_SECURITY
	Sysreg::VBAR::Write(Symbol::ExceptionVectorBase.Addr());
#else
	// TODO: Map exception vector base and set to high vectors.
#endif
#endif
/*
#ifdef ARM_EXT_SECURITY
	if(Sysreg::SCR::Read() & Sysreg::SCR::FLAG_NS)
		Console::WriteMessage(Console::Style::OK, "Secure state:", "Disabled");
	else
		Console::WriteMessage(Console::Style::WARNING, "Secure state:", "Enabled");
#endif
*/

	Console::WriteMessage(Console::Style::INFO, "Register arguments:", "r0 = 0x%8x, r1 = 0x%8x, r2 = 0x%8x", r0, r1, r2);
/*	Console::WriteFormat("SCTLR = 0x%8x\n", Sysreg::SCTLR::Read());
	Console::WriteFormat("MIDR = 0x%8x\n", Sysreg::MIDR::Read());
	Console::WriteFormat("VBAR = 0x%8x\n", Sysreg::VBAR::Read());
	Console::WriteFormat("CNTFRQ = %d\n", Sysreg::CNTFRQ::Read());
*/
	Console::WriteMessage(Console::Style::INFO, "Firmware revision:", "0x%8x", Mailbox::GetFirmwareRev());
	Console::WriteMessage(Console::Style::INFO, "Board model:", "0x%8x", Mailbox::GetBoardModel());
	Console::WriteMessage(Console::Style::INFO, "Board revision:", "0x%8x", Mailbox::GetBoardRev());

	armmem = Mailbox::GetArmMemory();
	vidmem = Mailbox::GetVideoMemory();

	Console::WriteMessage(Console::Style::INFO, "ARM memory:", "start = 0x%8x, length = 0x%8x", armmem.start, armmem.length);
	Console::WriteMessage(Console::Style::INFO, "Video memory:", "start = 0x%8x, length = 0x%8x", vidmem.start, vidmem.length);

	Chunker::Init(armmem.start, armmem.length, Memory::Zone::NONE);
	Chunker::Reserve(Pager::VirtToPhys(Symbol::kernelStart.Addr()), Pager::VirtToPhys(Symbol::kernelStart.Addr()) + Symbol::kernelEnd.Addr() - Symbol::kernelStart.Addr());

//	*((int*)0x12345678) = 0x87654321;

	Heap::Init();
	int* a = new int;
	Heap::ShowMap();
	int* b = new int;
	Heap::ShowMap();

	Console::WriteFormat("a = %p, b = %p\n", a, b);
/*
	uintptr_t virt = Symbol::kernelStart.Addr();
	do
	{
		Memory::PhysAddr phys = Pager::VirtToPhys(virt);
		if(phys != ~0UL)
			Console::WriteFormat("0x%p => 0x%8x\n", virt, phys);
		virt += 0x1000;
	} while(virt);
*/
	delete a;
	Heap::ShowMap();
	delete b;
	Heap::ShowMap();

#if CPU_COUNT > 1
	Processor::proc[0].state = Processor::State::Online;
	Processor::proc[1].state = Processor::State::Booting;
	Processor::proc[2].state = Processor::State::Booting;
	Processor::proc[3].state = Processor::State::Booting;

	apflag = 1;

#ifdef ELF64
	asm volatile ("dsb ish; sev");
#elif defined ARCH_ARM_V7
	asm volatile ("dsb; sev");
#endif

	unsigned int time = Timer::CounterLow();

	while((Timer::CounterLow() - time < 0x100000) && (Processor::proc[1].state == Processor::State::Booting || Processor::proc[2].state == Processor::State::Booting || Processor::proc[3].state == Processor::State::Booting))
		;

	for(int i = 0; i < 4; i++)
	{
		if(Processor::proc[i].state == Processor::State::Online)
		{
			Console::WriteMessage(Console::Style::OK, "Core %d: ", "booted", i);
		}
		else
		{
			Console::WriteMessage(Console::Style::WARNING, "Core %d: ", "no response, disabled", i);
			Processor::proc[i].state = Processor::State::Offline;
		}
	}
#endif
}

#if CPU_COUNT > 1
extern "C" void SECTION(".init.text") ApplicationEntry(void)
{
#ifdef ELF64
	unsigned int n = Sysreg::MPIDR_EL1::Read() & 0x3;
#else
	unsigned int n = Sysreg::MPIDR::Read() & 0x3;
#endif

	Processor::proc[n].state = Processor::State::Online;
}
#endif
