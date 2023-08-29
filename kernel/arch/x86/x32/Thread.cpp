// Thread.cpp - Threads for i386 architecture.

#include INC_BITS(Thread.h)
#include INC_ARCH(ControlRegisters.h)
#include <Console.h>

using namespace Kernel;

void ThreadData::Dump(void)
{
	Console::WriteFormat("EIP = 0x%8x; CR3 = 0x%8x; FLG = 0x%8x\n", eip, CR3::Read(), eflags);
	Console::WriteFormat("EAX = 0x%8x; EBX = 0x%8x; ECX = 0x%8x; EDX = 0x%8x\n", eax, ebx, ecx, edx);
	Console::WriteFormat("ESI = 0x%8x; EDI = 0x%8x; EBP = 0x%8x; ESP = 0x%8x\n", esi, edi, ebp, esp);
}
