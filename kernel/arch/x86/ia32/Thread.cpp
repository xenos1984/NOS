// Thread.cpp - Threads for i386 architecture.

#include INC_SUBARCH(Thread.h)
#include INC_ARCH(ControlRegisters.h)
#include <Console.h>

using namespace Kernel;

void ThreadData::Dump(void)
{
	console().WriteFormat("EIP = 0x%8x; CR3 = 0x%8x; FLG = 0x%8x\n", eip, CR3::Read(), eflags);
	console().WriteFormat("EAX = 0x%8x; EBX = 0x%8x; ECX = 0x%8x; EDX = 0x%8x\n", eax, ebx, ecx, edx);
	console().WriteFormat("ESI = 0x%8x; EDI = 0x%8x; EBP = 0x%8x; ESP = 0x%8x\n", esi, edi, ebp, esp);
}
