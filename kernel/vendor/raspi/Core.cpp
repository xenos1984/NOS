// Core.cpp - Core kernel implementation

#include <cstdint>
#include <Core.h>
#include INC_VENDOR(UART.h)

using namespace Kernel;

extern "C" void SECTION(".init.text") KernelEntry(uint32_t r0, uint32_t r1, uint32_t r2)
{
	UART::Init();
	Core::Welcome();
}

