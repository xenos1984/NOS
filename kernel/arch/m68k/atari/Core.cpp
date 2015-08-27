// Core.cpp - Core kernel implementation.

#include <new>
#include <Core.h>
#include <Console.h>
#include INC_ARCH(CPU.h)
#include INC_SUBARCH(SysVars.h)
#include INC_SUBARCH(MFP.h)
#include INC_SUBARCH(Video.h)
#include INC_SUBARCH(Interrupt.h)

using namespace Kernel;

extern "C" void SECTION(".init.text") KernelEntry(void)
{
	MFP::Init();
	Video::Init();
	Console::Init();
	Core::Welcome();

	intvects[0x02] = BusErrorWrapper;
	intvects[0x03] = AddressErrorWrapper;
	intvects[0x04] = IllegalInstrWrapper;
	intvects[0x05] = DivByZeroWrapper;

	Console::WriteMessage(Console::Style::INFO, "Total memory:", "%d kB", (sysvars.MemTop >> 10) + 32);

	*(unsigned long*)0xfa1234 = 1234;
}
