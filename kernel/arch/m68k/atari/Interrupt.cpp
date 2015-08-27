// Interupt.cpp - Interrupt handling

#include INC_SUBARCH(Interrupt.h)
#include INC_ARCH(CPU.h)
#include <Console.h>

void DumpRegisters(RegisterSet* regs)
{
	Console::WriteFormat("D0 = 0x%8x, A0 = 0x%8x\n", regs->d0, regs->a0);
	Console::WriteFormat("D1 = 0x%8x, A1 = 0x%8x\n", regs->d1, regs->a1);
	Console::WriteFormat("D2 = 0x%8x, A2 = 0x%8x\n", regs->d2, regs->a2);
	Console::WriteFormat("D3 = 0x%8x, A3 = 0x%8x\n", regs->d3, regs->a3);
	Console::WriteFormat("D4 = 0x%8x, A4 = 0x%8x\n", regs->d4, regs->a4);
	Console::WriteFormat("D5 = 0x%8x, A5 = 0x%8x\n", regs->d5, regs->a5);
	Console::WriteFormat("D6 = 0x%8x, A6 = 0x%8x\n", regs->d6, regs->a6);
	Console::WriteFormat("D7 = 0x%8x, A7 = 0x%8x\n", regs->d7, regs);
	Console::WriteFormat("SR = 0x%4x, USP = 0x%8x\n", regs->sr, regs->usp);
}

void DumpRegisters(RegisterSetEx* regs)
{
	Console::WriteFormat("D0 = 0x%8x, A0 = 0x%8x\n", regs->d0, regs->a0);
	Console::WriteFormat("D1 = 0x%8x, A1 = 0x%8x\n", regs->d1, regs->a1);
	Console::WriteFormat("D2 = 0x%8x, A2 = 0x%8x\n", regs->d2, regs->a2);
	Console::WriteFormat("D3 = 0x%8x, A3 = 0x%8x\n", regs->d3, regs->a3);
	Console::WriteFormat("D4 = 0x%8x, A4 = 0x%8x\n", regs->d4, regs->a4);
	Console::WriteFormat("D5 = 0x%8x, A5 = 0x%8x\n", regs->d5, regs->a5);
	Console::WriteFormat("D6 = 0x%8x, A6 = 0x%8x\n", regs->d6, regs->a6);
	Console::WriteFormat("D7 = 0x%8x, A7 = 0x%8x\n", regs->d7, regs);
	Console::WriteFormat("SR = 0x%4x, USP = 0x%8x\n", regs->sr, regs->usp);
	Console::WriteFormat("Instr = 0x%4x, Flags = 0x%4x\n", regs->ins, regs->flags);
	Console::WriteFormat("Addr = 0x%8x\n", regs->addr);
}

void BusError(RegisterSetEx* regs)
{
	Console::SetStyle(Console::Style::ERROR);
	Console::WriteFormat("BusError at PC = %8x\n", regs->pc);
	DumpRegisters(regs);
	Stop(0x2700);
}

void AddressError(RegisterSetEx* regs)
{
	Console::SetStyle(Console::Style::ERROR);
	Console::WriteFormat("AddressError at PC = %8x\n", regs->pc);
	DumpRegisters(regs);
	Stop(0x2700);
}

void IllegalInstr(RegisterSet* regs)
{
	Console::SetStyle(Console::Style::ERROR);
	Console::WriteFormat("IllegalInstr at PC = %8x\n", regs->pc);
	DumpRegisters(regs);
	Stop(0x2700);
}

void DivByZero(RegisterSet* regs)
{
	Console::SetStyle(Console::Style::ERROR);
	Console::WriteFormat("DivByZero at PC = %8x\n", regs->pc);
	DumpRegisters(regs);
}
