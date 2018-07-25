// Core.cpp - Core kernel implementation

#include <cstdint>
#include <Core.h>
#include <Console.h>
#include <Symbol.h>
#include <Pager.h>
#include <Chunker.h>
#include <Heap.h>
#include INC_VENDOR(UART.h)
#include INC_VENDOR(Mailbox.h)
#include INC_BITS(SystemRegs.h)

using namespace Kernel;

extern "C" void SECTION(".init.text") KernelEntry(uint32_t r0, uint32_t r1, uint32_t r2)
{
	Mailbox::MemInfo armmem, vidmem;

	UART::Init();
	Core::Welcome();

//	Sysreg::VBAR::Write(Symbol::ExceptionVectorBase.Addr());

	Console::WriteFormat("Register arguments: r0 = 0x%8x, r1 = 0x%8x, r2 = 0x%8x\n", r0, r1, r2);
/*	Console::WriteFormat("SCTLR = 0x%8x\n", Sysreg::SCTLR::Read());
	Console::WriteFormat("MIDR = 0x%8x\n", Sysreg::MIDR::Read());
	Console::WriteFormat("VBAR = 0x%8x\n", Sysreg::VBAR::Read());
	Console::WriteFormat("CNTFRQ = %d\n", Sysreg::CNTFRQ::Read());

	Console::WriteFormat("Firmware revision: 0x%8x\n", Mailbox::GetFirmwareRev());
	Console::WriteFormat("Board model: 0x%8x\n", Mailbox::GetBoardModel());
	Console::WriteFormat("Board revision: 0x%8x\n", Mailbox::GetBoardRev());
*/
	armmem = Mailbox::GetArmMemory();
	vidmem = Mailbox::GetVideoMemory();

	Console::WriteFormat("ARM memory: start = 0x%8x, length = 0x%8x\n", armmem.start, armmem.length);
	Console::WriteFormat("Video memory: start = 0x%8x, length = 0x%8x\n", vidmem.start, vidmem.length);

	Chunker::Init(armmem.start, armmem.length, Memory::Zone::NONE);
	Chunker::Reserve(Symbol::kernelStart.Addr() - Symbol::kernelOffset.Addr(), Symbol::kernelEnd.Addr() - Symbol::kernelOffset.Addr());
/*
	Heap::Init();
	int* a = new int;
	Heap::ShowMap();
	int* b = new int;
	Heap::ShowMap();
	delete a;
	Heap::ShowMap();
	delete b;
	Heap::ShowMap();
*/}
