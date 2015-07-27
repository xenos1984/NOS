// Registers.cpp - Register dump pushed on interrupt / exception.

#include INC_SUBARCH(Registers.h)
#include INC_ARCH(ControlRegisters.h)
#include <Console.h>

using namespace Kernel;

void RegisterSet::Dump(void)
{
	console().WriteFormat(" CS = 0x%8x; EIP = 0x%8x; CR3 = 0x%8x; FLG = 0x%8x\n", cs, eip, CR3::Read(), eflags);
	console().WriteFormat("EAX = 0x%8x; EBX = 0x%8x; ECX = 0x%8x; EDX = 0x%8x\n", eax, ebx, ecx, edx);
	console().WriteFormat("ESI = 0x%8x; EDI = 0x%8x; EBP = 0x%8x; ESP = 0x%8x\n", esi, edi, ebp, esp);
}

void SelectorErrorCode::Dump(void)
{
	console().WriteFormat("%s, %s, selector 0x%4x\n", (External ? "external" : "internal"), (DescriptorIDT ? "IDT" : (DescriptorLDT ? "LDT" : "GDT")), SelectorIndex << 3);
}

void PageFaultErrorCode::Dump(void)
{
	console().WriteFormat("CR2 = 0x%8x; %s, %s,\n", CR2::Read(), (Present ? "protection violation" : "non-present page"), (WriteAccess ? "write" : "read"));
	console().WriteFormat("%s, %s, %s\n", (UserMode ? "user" : "supervisor"), (ReservedBits ? "reserved bits" : "no reserved bits"), (InstructionFetch ? "instruction fetch" : "data access"));
}
