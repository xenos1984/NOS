// Interrupt.h - Interrupts and exceptions in ARM architecture.

#include INC_ARCH(Interrupt.h)
#include INC_ARCH(Coprocessor.h)
#include <Console.h>

using namespace Kernel;

void RegisterSet::Dump()
{
	Console::WriteFormat(" r0 = 0x%8x,  r1 = 0x%8x,  r2 = 0x%8x,  r3 = 0x%8x,\n", r0, r1, r2, r3);
	Console::WriteFormat(" r4 = 0x%8x,  r5 = 0x%8x,  r6 = 0x%8x,  r7 = 0x%8x,\n", r4, r5, r6, r7);
	Console::WriteFormat(" r8 = 0x%8x,  r9 = 0x%8x, r10 = 0x%8x, r11 = 0x%8x,\n", r8, r9, r10, r11);
	Console::WriteFormat("r12 = 0x%8x, usp = 0x%8x, ulr = 0x%8x,  pc = 0x%8x,\n", r12, usp, ulr, pc);
	Console::WriteFormat(" lr = 0x%8x, psr = 0x%8x.\n", lr, spsr);
}

extern "C" void DataAbort(RegisterSet* regs)
{
	Console::WriteFormat("Data abort occurred\n");
	Console::WriteFormat("DFAR = 0x%8x, DFSR = 0x%8x\n", Coprocessor::DFAR::Read(), Coprocessor::DFSR::Read());
	regs->Dump();
}
