// Core.cpp - Core kernel implementation

#include <cstdint>
#include <Core.h>
#include <Console.h>
#include INC_VENDOR(UART.h)
#include INC_ARCH(Coprocessor.h)

using namespace Kernel;

extern "C" void SECTION(".init.text") KernelEntry(uint32_t r0, uint32_t r1, uint32_t r2)
{
	UART::Init();
	Core::Welcome();

	Console::WriteFormat("Register arguments: r0 = 0x%8x, r1 = 0x%8x, r2 = 0x%8x\n", r0, r1, r2);
	Console::WriteFormat("SCTLR = 0x%8x\n", Coprocessor::SCTLR::Read());
	Console::WriteFormat("MIDR = 0x%8x\n", Coprocessor::MIDR::Read());
}

