// Registers.cpp - Register dump pushed on interrupt / exception.

#include INC_SUBARCH(Registers.h)
#include INC_ARCH(ControlRegisters.h)
#include <Console.h>

using namespace Kernel;

void RegisterSet::Dump(void)
{
	console().WriteFormat(" CS = 0x%16lx; RIP = 0x%16lx\n", cs, rip);
	console().WriteFormat("RAX = 0x%16lx; RBX = 0x%16lx\n", rax, rbx);
	console().WriteFormat("RCX = 0x%16lx; RDX = 0x%16lx\n", rcx, rdx);
	console().WriteFormat("RSI = 0x%16lx; RDI = 0x%16lx\n", rsi, rdi);
	console().WriteFormat("RBP = 0x%16lx; RSP = 0x%16lx\n", rbp, rsp);
	console().WriteFormat("R8  = 0x%16lx; R9  = 0x%16lx\n", r8, r9);
	console().WriteFormat("R10 = 0x%16lx; R11 = 0x%16lx\n", r10, r11);
	console().WriteFormat("R12 = 0x%16lx; R13 = 0x%16lx\n", r12, r13);
	console().WriteFormat("R14 = 0x%16lx; R15 = 0x%16lx\n", r14, r15);
	console().WriteFormat("CR3 = 0x%16lx; FLG = 0x%16lx\n", CR3::Read(), rflags);
}

void SelectorErrorCode::Dump(void)
{
	console().WriteFormat("%s, %s, selector 0x%4x\n", (External ? "external" : "internal"), (DescriptorIDT ? "IDT" : (DescriptorLDT ? "LDT" : "GDT")), SelectorIndex << 3);
}

void PageFaultErrorCode::Dump(void)
{
	console().WriteFormat("CR2 = 0x%16lx; %s, %s,\n", CR2::Read(), (Present ? "protection violation" : "non-present page"), (WriteAccess ? "write" : "read"));
	console().WriteFormat("%s, %s, %s\n", (UserMode ? "user" : "supervisor"), (ReservedBits ? "reserved bits" : "no reserved bits"), (InstructionFetch ? "instruction fetch" : "data access"));
}
