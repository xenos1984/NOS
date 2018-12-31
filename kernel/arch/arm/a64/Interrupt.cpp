// Interrupt.h - Interrupts and exceptions in ARM architecture.

#include INC_BITS(Interrupt.h)
#include INC_BITS(SystemRegs.h)
//#include INC_SUBARCH(PageTable.h)
#include <Console.h>

namespace Kernel
{
}

using namespace Kernel;

void RegisterSet::Dump()
{
	Console::WriteFormat(" r0 = 0x%16lx,  r1 = 0x%16lx,  r2 = 0x%16lx,\n", r0, r1, r2);
	Console::WriteFormat(" r3 = 0x%16lx,  r4 = 0x%16lx,  r5 = 0x%16lx,\n", r3, r4, r5);
	Console::WriteFormat(" r6 = 0x%16lx,  r7 = 0x%16lx,  r8 = 0x%16lx,\n", r6, r7, r8);
	Console::WriteFormat(" r9 = 0x%16lx, r10 = 0x%16lx, r11 = 0x%16lx,\n", r9, r10, r11);
	Console::WriteFormat("r12 = 0x%16lx, r13 = 0x%16lx, r14 = 0x%16lx,\n", r12, r13, r14);
	Console::WriteFormat("r15 = 0x%16lx, r16 = 0x%16lx, r17 = 0x%16lx,\n", r15, r16, r17);
	Console::WriteFormat("r18 = 0x%16lx, r19 = 0x%16lx, r20 = 0x%16lx,\n", r18, r19, r20);
	Console::WriteFormat("r21 = 0x%16lx, r22 = 0x%16lx, r23 = 0x%16lx,\n", r21, r22, r23);
	Console::WriteFormat("r24 = 0x%16lx, r25 = 0x%16lx, r26 = 0x%16lx,\n", r24, r25, r26);
	Console::WriteFormat("r27 = 0x%16lx, r28 = 0x%16lx, r29 = 0x%16lx,\n", r27, r28, r29);
	Console::WriteFormat(" lr = 0x%16lx,  sp = 0x%16lx.\n", lr, sp);
}

extern "C" void KernelSyncHandler(RegisterSet* regs)
{
	uint64_t syndrome = Sysreg::ESR_EL1::Read();

	Console::WriteFormat("Synchronous exception occurred with ESR = 0x%8x:\n", syndrome);
	regs->Dump();
}
