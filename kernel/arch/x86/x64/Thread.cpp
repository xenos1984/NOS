// Thread.cpp - Threads for x64 architecture.

#include INC_BITS(Thread.h)
#include INC_ARCH(ControlRegisters.h)
#include <Console.h>

using namespace Kernel;

void ThreadData::Dump(void)
{
	Console::WriteFormat("RIP = 0x%16lx\n", rip);
	Console::WriteFormat("RAX = 0x%16lx; RBX = 0x%16lx\n", rax, rbx);
	Console::WriteFormat("RCX = 0x%16lx; RDX = 0x%16lx\n", rcx, rdx);
	Console::WriteFormat("RSI = 0x%16lx; RDI = 0x%16lx\n", rsi, rdi);
	Console::WriteFormat("RBP = 0x%16lx; RSP = 0x%16lx\n", rbp, rsp);
	Console::WriteFormat("R8  = 0x%16lx; R9  = 0x%16lx\n", r8, r9);
	Console::WriteFormat("R10 = 0x%16lx; R11 = 0x%16lx\n", r10, r11);
	Console::WriteFormat("R12 = 0x%16lx; R13 = 0x%16lx\n", r12, r13);
	Console::WriteFormat("R14 = 0x%16lx; R15 = 0x%16lx\n", r14, r15);
	Console::WriteFormat("CR3 = 0x%16lx; FLG = 0x%16lx\n", CR3::Read(), rflags);
}
