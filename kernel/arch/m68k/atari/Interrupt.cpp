// Interupt.cpp - Interrupt handling

#include INC_SUBARCH(Interrupt.h)
#include INC_ARCH(CPU.h)
#include <Console.h>

void DumpRegisters(RegisterSet* regs)
{
	console().WriteFormat("D0 = 0x%8x, A0 = 0x%8x\n", regs->d0, regs->a0);
	console().WriteFormat("D1 = 0x%8x, A1 = 0x%8x\n", regs->d1, regs->a1);
	console().WriteFormat("D2 = 0x%8x, A2 = 0x%8x\n", regs->d2, regs->a2);
	console().WriteFormat("D3 = 0x%8x, A3 = 0x%8x\n", regs->d3, regs->a3);
	console().WriteFormat("D4 = 0x%8x, A4 = 0x%8x\n", regs->d4, regs->a4);
	console().WriteFormat("D5 = 0x%8x, A5 = 0x%8x\n", regs->d5, regs->a5);
	console().WriteFormat("D6 = 0x%8x, A6 = 0x%8x\n", regs->d6, regs->a6);
	console().WriteFormat("D7 = 0x%8x, A7 = 0x%8x\n", regs->d7, regs);
	console().WriteFormat("SR = 0x%4x, USP = 0x%8x\n", regs->sr, regs->usp);
}

void DumpRegisters(RegisterSetEx* regs)
{
	console().WriteFormat("D0 = 0x%8x, A0 = 0x%8x\n", regs->d0, regs->a0);
	console().WriteFormat("D1 = 0x%8x, A1 = 0x%8x\n", regs->d1, regs->a1);
	console().WriteFormat("D2 = 0x%8x, A2 = 0x%8x\n", regs->d2, regs->a2);
	console().WriteFormat("D3 = 0x%8x, A3 = 0x%8x\n", regs->d3, regs->a3);
	console().WriteFormat("D4 = 0x%8x, A4 = 0x%8x\n", regs->d4, regs->a4);
	console().WriteFormat("D5 = 0x%8x, A5 = 0x%8x\n", regs->d5, regs->a5);
	console().WriteFormat("D6 = 0x%8x, A6 = 0x%8x\n", regs->d6, regs->a6);
	console().WriteFormat("D7 = 0x%8x, A7 = 0x%8x\n", regs->d7, regs);
	console().WriteFormat("SR = 0x%4x, USP = 0x%8x\n", regs->sr, regs->usp);
	console().WriteFormat("Instr = 0x%4x, Flags = 0x%4x\n", regs->ins, regs->flags);
	console().WriteFormat("Addr = 0x%8x\n", regs->addr);
}

void BusError(RegisterSetEx* regs)
{
	console().WriteFormat("BusError at PC = %8x\n", regs->pc);
	DumpRegisters(regs);
	Stop(0x2700);
}

void AddressError(RegisterSetEx* regs)
{
	console().WriteFormat("AddressError at PC = %8x\n", regs->pc);
	DumpRegisters(regs);
	Stop(0x2700);
}

void IllegalInstr(RegisterSet* regs)
{
	console().WriteFormat("IllegalInstr at PC = %8x\n", regs->pc);
	DumpRegisters(regs);
	Stop(0x2700);
}

void DivByZero(RegisterSet* regs)
{
	console().WriteFormat("DivByZero at PC = %8x\n", regs->pc);
	DumpRegisters(regs);
}
