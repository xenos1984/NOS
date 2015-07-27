// Interupt.cpp - Interrupt handling

#include <cstddef>
#include INC_ARCH(Interrupt.h)
#include INC_SUBARCH(PMode.h)
#include INC_ARCH(ControlRegisters.h)
#include INC_ARCH(Apic.h)
#include INC_ARCH(CPU.h)
#include INC_ARCH(FPU.h)
#include INC_ARCH(Cmos.h)
#include <Thread.h>
#include <Process.h>
#include <Console.h>
#include <TaskScheduler.h>
#include <VirtualMemory.h>
#include <IRQManager.h>

#define USERINT(name) void User##name(void)
#define KERNINT(name) void Kernel##name(RegisterSet* regs)

#define USEREXC(name) void User##name(SelectorErrorCode error)
#define KERNEXC(name) void Kernel##name(RegisterSet* regs, SelectorErrorCode error)

#define USERPAG(name) void User##name(PageFaultErrorCode error)
#define KERNPAG(name) void Kernel##name(RegisterSet* regs, PageFaultErrorCode error)

#define TIMER(name) void name(void)

using namespace Kernel;

// User level trap handlers

USERINT(DivideError)
{
	console().SetForeground(Console::FG_YELLOW);
	console().WriteFormat("DivideError occured\n");
	tasksched().GetCurrent()->Dump();
	Halt();
}

USERINT(DebugInterrupt)
{
	console().SetForeground(Console::FG_YELLOW);
	console().WriteFormat("DebugInterrupt occured\n");
	tasksched().GetCurrent()->Dump();
	Halt();
}

USERINT(NonMaskable)
{
	console().SetForeground(Console::FG_YELLOW);
	console().WriteFormat("NonMaskable occured\n");
	tasksched().GetCurrent()->Dump();
	Halt();
}

USERINT(BreakPoint)
{
	console().SetForeground(Console::FG_YELLOW);
	console().WriteFormat("BreakPoint occured\n");
	tasksched().GetCurrent()->Dump();
	Halt();
}

USERINT(Overflow)
{
	console().SetForeground(Console::FG_YELLOW);
	console().WriteFormat("Overflow occured\n");
	tasksched().GetCurrent()->Dump();
	Halt();
}

USERINT(Arraybounds)
{
	console().SetForeground(Console::FG_YELLOW);
	console().WriteFormat("Arraybounds occured\n");
	tasksched().GetCurrent()->Dump();
	Halt();
}

USERINT(InvalidOpcode)
{
	console().SetForeground(Console::FG_YELLOW);
	console().WriteFormat("InvalidOpcode occured\n");
	tasksched().GetCurrent()->Dump();
	Halt();
}

USERINT(DeviceNotAvailable)
{
	Thread* t = tasksched().GetCurrent();

	CR0::Write(CR0::Read() & ~CR0::TASK_SWITCHED);

	if(t->data.mmstate == nullptr)
	{
		t->data.mmstate = MMState::Create();
		asm volatile ("finit");
	}
	else
	{
		t->data.mmstate->Restore();
	}
}

USEREXC(DoubleFault)
{
	console().SetForeground(Console::FG_YELLOW);
	console().WriteFormat("DoubleFault occured\n");
	error.Dump();
	tasksched().GetCurrent()->Dump();
	Halt();
}

USERINT(CoprocSegOverrun)
{
	console().SetForeground(Console::FG_YELLOW);
	console().WriteFormat("CoprocSegOverrun occured\n");
	tasksched().GetCurrent()->Dump();
	Halt();
}

USEREXC(InvalidTSS)
{
	console().SetForeground(Console::FG_YELLOW);
	console().WriteFormat("InvalidTSS occured\n");
	error.Dump();
	tasksched().GetCurrent()->Dump();
	Halt();
}

USEREXC(SegmentNotPresent)
{
	console().SetForeground(Console::FG_YELLOW);
	console().WriteFormat("SegmentNotPresent occured\n");
	error.Dump();
	tasksched().GetCurrent()->Dump();
	Halt();
}

USEREXC(StackFault)
{
	console().SetForeground(Console::FG_YELLOW);
	console().WriteFormat("StackFault occured\n");
	error.Dump();
	tasksched().GetCurrent()->Dump();
	Halt();
}

USEREXC(GeneralProtection)
{
/*	bool opsize = false, addrsize = false;
	Thread* t = tasksched().GetCurrent();
	unsigned char* ip = (unsigned char*)t->data.eip;
*/
	console().SetForeground(Console::FG_YELLOW);
	console().WriteFormat("GeneralProtection occured\n");
/*
	reason:
	switch(ip[0])
	{
	case 0x66:
		opsize = true;
		ip++;
		goto reason;
		break;

	case 0x67:
		addrsize = true;
		ip++;
		goto reason;
		break;

	case 0xe4:
		console().WriteFormat("Illegal I/O: inb $0x%2x, %%al\n", ip[1]);
		break;

	case 0xe5:
		if(opsize)
			console().WriteFormat("Illegal I/O: inw $0x%2x, %%ax\n", ip[1]);
		else
			console().WriteFormat("Illegal I/O: inl $0x%2x, %%eax\n", ip[1]);
		break;

	case 0xec:
		console().WriteFormat("Illegal I/O: inb %%dx, %%al with %%dx = 0x%4x\n", t->data.edx & 0xffff);
		break;

	case 0xed:
		if(opsize)
			console().WriteFormat("Illegal I/O: inw %%dx, %%ax with %%dx = 0x%4x\n", t->data.edx & 0xffff);
		else
			console().WriteFormat("Illegal I/O: inl %%dx, %%eax with %%dx = 0x%4x\n", t->data.edx & 0xffff);
		break;

	case 0xe6:
		console().WriteFormat("Illegal I/O: outb %%al, $0x%2x\n", ip[1]);
		break;

	case 0xe7:
		if(opsize)
			console().WriteFormat("Illegal I/O: outw %%ax, $0x%2x\n", ip[1]);
		else
			console().WriteFormat("Illegal I/O: outl %%eax, $0x%2x\n", ip[1]);
		break;

	case 0xee:
		console().WriteFormat("Illegal I/O: outb %%al, %%dx with %%dx = 0x%4x\n", t->data.edx & 0xffff);
		break;

	case 0xef:
		if(opsize)
			console().WriteFormat("Illegal I/O: outw %%ax, %%dx with %%dx = 0x%4x\n", t->data.edx & 0xffff);
		else
			console().WriteFormat("Illegal I/O: outl %%eax, %%dx with %%dx = 0x%4x\n", t->data.edx & 0xffff);
		break;

	case 0x6c:
		console().WriteFormat("Illegal I/O: insb with %%dx = 0x%4x\n", t->data.edx & 0xffff);
		break;

	case 0x6d:
		console().WriteFormat("Illegal I/O: %s with %%dx = 0x%4x\n", (opsize ? "insw" : "insl"), t->data.edx & 0xffff);
		break;

	case 0x6e:
		console().WriteFormat("Illegal I/O: outsb with %%dx = 0x%4x\n", t->data.edx & 0xffff);
		break;

	case 0x6f:
		console().WriteFormat("Illegal I/O: %s with %%dx = 0x%4x\n", (opsize ? "outsw" : "outsl"), t->data.edx & 0xffff);
		break;

	case 0xf4:
		console().WriteFormat("Illegal instruction: hlt\n");
		break;

	case 0xfa:
		console().WriteFormat("Illegal instruction: cli\n");
		break;

	case 0xfb:
		console().WriteFormat("Illegal instruction: sti\n");
		break;

	default:
		break;
	}
*/
	error.Dump();
	tasksched().GetCurrent()->Dump();
	Halt();
}

USERPAG(PageFault)
{
	console().SetForeground(Console::FG_YELLOW);
	console().WriteFormat("PageFault occured\n");
	error.Dump();
	tasksched().GetCurrent()->Dump();
	Halt();
}

USERINT(CoprocError)
{
	console().SetForeground(Console::FG_YELLOW);
	console().WriteFormat("CoprocError occured\n");
	tasksched().GetCurrent()->Dump();
	Halt();
}

USEREXC(AlignmentCheck)
{
	console().SetForeground(Console::FG_YELLOW);
	console().WriteFormat("AlignmentCheck occured\n");
	error.Dump();
	tasksched().GetCurrent()->Dump();
	Halt();
}

USERINT(MachineCheck)
{
	console().SetForeground(Console::FG_YELLOW);
	console().WriteFormat("MachineCheck occured\n");
	tasksched().GetCurrent()->Dump();
	Halt();
}

USERINT(SSEFault)
{
	console().SetForeground(Console::FG_YELLOW);
	console().WriteFormat("SSEFault occured\n");
	tasksched().GetCurrent()->Dump();
	Halt();
}

// Kernel level trap handlers - this should not happen and indicates a kernel bug

KERNINT(DivideError)
{
	console().SetForeground(Console::FG_RED);
	console().WriteFormat("DivideError occured\n");
	regs->Dump();
}

KERNINT(DebugInterrupt)
{
	console().SetForeground(Console::FG_RED);
	console().WriteFormat("DebugInterrupt occured\n");
	regs->Dump();
}

KERNINT(NonMaskable)
{
	console().SetForeground(Console::FG_RED);
	console().WriteFormat("NonMaskable occured\n");
	regs->Dump();
}

KERNINT(BreakPoint)
{
	console().SetForeground(Console::FG_RED);
	console().WriteFormat("BreakPoint occured\n");
	regs->Dump();
}

KERNINT(Overflow)
{
	console().SetForeground(Console::FG_RED);
	console().WriteFormat("Overflow occured\n");
	regs->Dump();
}

KERNINT(Arraybounds)
{
	console().SetForeground(Console::FG_RED);
	console().WriteFormat("Arraybounds occured\n");
	regs->Dump();
}

KERNINT(InvalidOpcode)
{
	console().SetForeground(Console::FG_RED);
	console().WriteFormat("InvalidOpcode occured\n");
	regs->Dump();
}

KERNINT(DeviceNotAvailable)
{
	console().SetForeground(Console::FG_RED);
	console().WriteFormat("DeviceNotAvailable occured\n");
	regs->Dump();
}

KERNEXC(DoubleFault)
{
	console().SetForeground(Console::FG_RED);
	console().WriteFormat("DoubleFault occured\n");
	error.Dump();
	regs->Dump();
}

KERNINT(CoprocSegOverrun)
{
	console().SetForeground(Console::FG_RED);
	console().WriteFormat("CoprocSegOverrun occured\n");
	regs->Dump();
}

KERNEXC(InvalidTSS)
{
	console().SetForeground(Console::FG_RED);
	console().WriteFormat("InvalidTSS occured\n");
	error.Dump();
	regs->Dump();
}

KERNEXC(SegmentNotPresent)
{
	console().SetForeground(Console::FG_RED);
	console().WriteFormat("SegmentNotPresent occured\n");
	error.Dump();
	regs->Dump();
}

KERNEXC(StackFault)
{
	console().SetForeground(Console::FG_RED);
	console().WriteFormat("StackFault occured\n");
	error.Dump();
	regs->Dump();
}

KERNEXC(GeneralProtection)
{
	console().SetForeground(Console::FG_RED);
	console().WriteFormat("GeneralProtection occured\n");
	error.Dump();
	regs->Dump();
}

KERNPAG(PageFault)
{
	console().SetForeground(Console::FG_RED);
	console().WriteFormat("PageFault occured\n");
	error.Dump();
	regs->Dump();
}

KERNINT(CoprocError)
{
	console().SetForeground(Console::FG_RED);
	console().WriteFormat("CoprocError occured\n");
	regs->Dump();
}

KERNEXC(AlignmentCheck)
{
	console().SetForeground(Console::FG_RED);
	console().WriteFormat("AlignmentCheck occured\n");
	error.Dump();
	regs->Dump();
}

KERNINT(MachineCheck)
{
	console().SetForeground(Console::FG_RED);
	console().WriteFormat("MachineCheck occured\n");
	regs->Dump();
}

KERNINT(SSEFault)
{
	console().SetForeground(Console::FG_RED);
	console().WriteFormat("SSEFault occured\n");
	regs->Dump();
}

// Hardware interrupt handlers

TIMER(CMOSTimerIRQ)
{
	tasksched().Schedule();
	cmos().GetStatusC();
	irqman().SendEOI(8);
}

TIMER(ApicTimerIRQ)
{
	tasksched().Schedule();
	apic().SignalEOI();
}

extern "C" void HardwareHandler(int num)
{
	irqman().HandleInterrupt(num);
}
