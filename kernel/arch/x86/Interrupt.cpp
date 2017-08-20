// Interupt.cpp - Interrupt handling

#include <cstddef>
#include INC_ARCH(Interrupt.h)
#include INC_SUBARCH(PMode.h)
#include INC_ARCH(ControlRegisters.h)
#include INC_ARCH(Apic.h)
#include INC_ARCH(CPU.h)
#include INC_ARCH(FPU.h)
#include INC_VENDOR(Cmos.h)
#include <Thread.h>
#include <Process.h>
#include <Console.h>
#include <TaskScheduler.h>
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
	Console::SetStyle(Console::Style::WARNING);
	Console::WriteFormat("DivideError occured\n");
	tasksched().GetCurrent()->Dump();
	Halt();
}

USERINT(DebugInterrupt)
{
	Console::SetStyle(Console::Style::WARNING);
	Console::WriteFormat("DebugInterrupt occured\n");
	tasksched().GetCurrent()->Dump();
	Halt();
}

USERINT(NonMaskable)
{
	Console::SetStyle(Console::Style::WARNING);
	Console::WriteFormat("NonMaskable occured\n");
	tasksched().GetCurrent()->Dump();
	Halt();
}

USERINT(BreakPoint)
{
	Console::SetStyle(Console::Style::WARNING);
	Console::WriteFormat("BreakPoint occured\n");
	tasksched().GetCurrent()->Dump();
	Halt();
}

USERINT(Overflow)
{
	Console::SetStyle(Console::Style::WARNING);
	Console::WriteFormat("Overflow occured\n");
	tasksched().GetCurrent()->Dump();
	Halt();
}

USERINT(Arraybounds)
{
	Console::SetStyle(Console::Style::WARNING);
	Console::WriteFormat("Arraybounds occured\n");
	tasksched().GetCurrent()->Dump();
	Halt();
}

USERINT(InvalidOpcode)
{
	Console::SetStyle(Console::Style::WARNING);
	Console::WriteFormat("InvalidOpcode occured\n");
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
	Console::SetStyle(Console::Style::WARNING);
	Console::WriteFormat("DoubleFault occured\n");
	error.Dump();
	tasksched().GetCurrent()->Dump();
	Halt();
}

USERINT(CoprocSegOverrun)
{
	Console::SetStyle(Console::Style::WARNING);
	Console::WriteFormat("CoprocSegOverrun occured\n");
	tasksched().GetCurrent()->Dump();
	Halt();
}

USEREXC(InvalidTSS)
{
	Console::SetStyle(Console::Style::WARNING);
	Console::WriteFormat("InvalidTSS occured\n");
	error.Dump();
	tasksched().GetCurrent()->Dump();
	Halt();
}

USEREXC(SegmentNotPresent)
{
	Console::SetStyle(Console::Style::WARNING);
	Console::WriteFormat("SegmentNotPresent occured\n");
	error.Dump();
	tasksched().GetCurrent()->Dump();
	Halt();
}

USEREXC(StackFault)
{
	Console::SetStyle(Console::Style::WARNING);
	Console::WriteFormat("StackFault occured\n");
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
	Console::SetStyle(Console::Style::WARNING);
	Console::WriteFormat("GeneralProtection occured\n");
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
		Console::WriteFormat("Illegal I/O: inb $0x%2x, %%al\n", ip[1]);
		break;

	case 0xe5:
		if(opsize)
			Console::WriteFormat("Illegal I/O: inw $0x%2x, %%ax\n", ip[1]);
		else
			Console::WriteFormat("Illegal I/O: inl $0x%2x, %%eax\n", ip[1]);
		break;

	case 0xec:
		Console::WriteFormat("Illegal I/O: inb %%dx, %%al with %%dx = 0x%4x\n", t->data.edx & 0xffff);
		break;

	case 0xed:
		if(opsize)
			Console::WriteFormat("Illegal I/O: inw %%dx, %%ax with %%dx = 0x%4x\n", t->data.edx & 0xffff);
		else
			Console::WriteFormat("Illegal I/O: inl %%dx, %%eax with %%dx = 0x%4x\n", t->data.edx & 0xffff);
		break;

	case 0xe6:
		Console::WriteFormat("Illegal I/O: outb %%al, $0x%2x\n", ip[1]);
		break;

	case 0xe7:
		if(opsize)
			Console::WriteFormat("Illegal I/O: outw %%ax, $0x%2x\n", ip[1]);
		else
			Console::WriteFormat("Illegal I/O: outl %%eax, $0x%2x\n", ip[1]);
		break;

	case 0xee:
		Console::WriteFormat("Illegal I/O: outb %%al, %%dx with %%dx = 0x%4x\n", t->data.edx & 0xffff);
		break;

	case 0xef:
		if(opsize)
			Console::WriteFormat("Illegal I/O: outw %%ax, %%dx with %%dx = 0x%4x\n", t->data.edx & 0xffff);
		else
			Console::WriteFormat("Illegal I/O: outl %%eax, %%dx with %%dx = 0x%4x\n", t->data.edx & 0xffff);
		break;

	case 0x6c:
		Console::WriteFormat("Illegal I/O: insb with %%dx = 0x%4x\n", t->data.edx & 0xffff);
		break;

	case 0x6d:
		Console::WriteFormat("Illegal I/O: %s with %%dx = 0x%4x\n", (opsize ? "insw" : "insl"), t->data.edx & 0xffff);
		break;

	case 0x6e:
		Console::WriteFormat("Illegal I/O: outsb with %%dx = 0x%4x\n", t->data.edx & 0xffff);
		break;

	case 0x6f:
		Console::WriteFormat("Illegal I/O: %s with %%dx = 0x%4x\n", (opsize ? "outsw" : "outsl"), t->data.edx & 0xffff);
		break;

	case 0xf4:
		Console::WriteFormat("Illegal instruction: hlt\n");
		break;

	case 0xfa:
		Console::WriteFormat("Illegal instruction: cli\n");
		break;

	case 0xfb:
		Console::WriteFormat("Illegal instruction: sti\n");
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
	Console::SetStyle(Console::Style::WARNING);
	Console::WriteFormat("PageFault occured\n");
	error.Dump();
	tasksched().GetCurrent()->Dump();
	Halt();
}

USERINT(CoprocError)
{
	Console::SetStyle(Console::Style::WARNING);
	Console::WriteFormat("CoprocError occured\n");
	tasksched().GetCurrent()->Dump();
	Halt();
}

USEREXC(AlignmentCheck)
{
	Console::SetStyle(Console::Style::WARNING);
	Console::WriteFormat("AlignmentCheck occured\n");
	error.Dump();
	tasksched().GetCurrent()->Dump();
	Halt();
}

USERINT(MachineCheck)
{
	Console::SetStyle(Console::Style::WARNING);
	Console::WriteFormat("MachineCheck occured\n");
	tasksched().GetCurrent()->Dump();
	Halt();
}

USERINT(SSEFault)
{
	Console::SetStyle(Console::Style::WARNING);
	Console::WriteFormat("SSEFault occured\n");
	tasksched().GetCurrent()->Dump();
	Halt();
}

// Kernel level trap handlers - this should not happen and indicates a kernel bug

KERNINT(DivideError)
{
	Console::SetStyle(Console::Style::ERROR);
	Console::WriteFormat("DivideError occured\n");
	regs->Dump();
}

KERNINT(DebugInterrupt)
{
	Console::SetStyle(Console::Style::ERROR);
	Console::WriteFormat("DebugInterrupt occured\n");
	regs->Dump();
}

KERNINT(NonMaskable)
{
	Console::SetStyle(Console::Style::ERROR);
	Console::WriteFormat("NonMaskable occured\n");
	regs->Dump();
}

KERNINT(BreakPoint)
{
	Console::SetStyle(Console::Style::ERROR);
	Console::WriteFormat("BreakPoint occured\n");
	regs->Dump();
}

KERNINT(Overflow)
{
	Console::SetStyle(Console::Style::ERROR);
	Console::WriteFormat("Overflow occured\n");
	regs->Dump();
}

KERNINT(Arraybounds)
{
	Console::SetStyle(Console::Style::ERROR);
	Console::WriteFormat("Arraybounds occured\n");
	regs->Dump();
}

KERNINT(InvalidOpcode)
{
	Console::SetStyle(Console::Style::ERROR);
	Console::WriteFormat("InvalidOpcode occured\n");
	regs->Dump();
}

KERNINT(DeviceNotAvailable)
{
	Console::SetStyle(Console::Style::ERROR);
	Console::WriteFormat("DeviceNotAvailable occured\n");
	regs->Dump();
}

KERNEXC(DoubleFault)
{
	Console::SetStyle(Console::Style::ERROR);
	Console::WriteFormat("DoubleFault occured\n");
	error.Dump();
	regs->Dump();
}

KERNINT(CoprocSegOverrun)
{
	Console::SetStyle(Console::Style::ERROR);
	Console::WriteFormat("CoprocSegOverrun occured\n");
	regs->Dump();
}

KERNEXC(InvalidTSS)
{
	Console::SetStyle(Console::Style::ERROR);
	Console::WriteFormat("InvalidTSS occured\n");
	error.Dump();
	regs->Dump();
}

KERNEXC(SegmentNotPresent)
{
	Console::SetStyle(Console::Style::ERROR);
	Console::WriteFormat("SegmentNotPresent occured\n");
	error.Dump();
	regs->Dump();
}

KERNEXC(StackFault)
{
	Console::SetStyle(Console::Style::ERROR);
	Console::WriteFormat("StackFault occured\n");
	error.Dump();
	regs->Dump();
}

KERNEXC(GeneralProtection)
{
	Console::SetStyle(Console::Style::ERROR);
	Console::WriteFormat("GeneralProtection occured\n");
	error.Dump();
	regs->Dump();
}

KERNPAG(PageFault)
{
	Console::SetStyle(Console::Style::ERROR);
	Console::WriteFormat("PageFault occured\n");
	error.Dump();
	regs->Dump();
}

KERNINT(CoprocError)
{
	Console::SetStyle(Console::Style::ERROR);
	Console::WriteFormat("CoprocError occured\n");
	regs->Dump();
}

KERNEXC(AlignmentCheck)
{
	Console::SetStyle(Console::Style::ERROR);
	Console::WriteFormat("AlignmentCheck occured\n");
	error.Dump();
	regs->Dump();
}

KERNINT(MachineCheck)
{
	Console::SetStyle(Console::Style::ERROR);
	Console::WriteFormat("MachineCheck occured\n");
	regs->Dump();
}

KERNINT(SSEFault)
{
	Console::SetStyle(Console::Style::ERROR);
	Console::WriteFormat("SSEFault occured\n");
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
	Apic::SignalEOI();
}

extern "C" void HardwareHandler(int num)
{
	irqman().HandleInterrupt(num);
}
