// IOApic.cpp - I/O Apic support

#include INC_ARCH(IOApic.h)
#include <Console.h>
#include <PhysicalMemory.h>
#include <VirtualMemory.h>

using namespace Kernel;

SECTION(".init.text") IOApic::IOApic(unsigned long addr, unsigned int base)
{
	unsigned int i;
	regsel = (volatile uint32_t*)physmem().MapToLinear((void*)addr, virtmem().Alloc(4096, 4096, false), 1);
	win = (volatile uint32_t*)(0x10 + (unsigned long)regsel);
	irqbase = base;
	irqcount = GetMaxEntry() + 1;

	for(i = 0; i < irqcount; i++)
	{
		if(irqbase + i < 16)
			SetIRQ(i, 0xff, irqbase + i + 0x20, INT_POL_HIGH | INT_EDGETRIG | INT_MASK | DEST_LOGICAL | DM_LOWPRI);
		else
			SetIRQ(i, 0xff, irqbase + i + 0x20, INT_POL_LOW | INT_LEVELTRIG | INT_MASK | DEST_LOGICAL | DM_LOWPRI);
	}

	console().WriteMessage(Console::MSG_INFO, "I/O APIC:", "at 0x%8x, Version 0x%2x, IRQ 0x%2x - 0x%2x", addr, GetVersion(), irqbase, irqbase + irqcount - 1);
}

unsigned int IOApic::GetID(void)
{
	unsigned int tmp;

	lock.Enter();
	*regsel = APICID;
	tmp = *win;
	lock.Exit();
	return(tmp);
}

void IOApic::SetID(unsigned int id)
{
	lock.Enter();
	*regsel = APICID;
	*win = id;
	lock.Exit();
}

unsigned char IOApic::GetVersion(void)
{
	unsigned char tmp;

	lock.Enter();
	*regsel = APICVER;
	tmp = *win & 0xff;
	lock.Exit();
	return(tmp);
}

unsigned char IOApic::GetMaxEntry(void)
{
	unsigned char tmp;

	lock.Enter();
	*regsel = APICVER;
	tmp = (*win >> 16) & 0xff;
	lock.Exit();
	return(tmp);
}

unsigned int IOApic::GetArbitration(void)
{
	unsigned int tmp;

	lock.Enter();
	*regsel = APICARB;
	tmp = *win;
	lock.Exit();
	return(tmp);
}

void IOApic::SetIRQ(unsigned char n, unsigned char dest, unsigned char vect, unsigned int flags)
{
	unsigned int tmp;

	lock.Enter();
	*regsel = REDTBL + 2 * n + 1;
	tmp = *win;
	tmp = (tmp & 0x00ffffff) | ((unsigned int)dest << 24);
	*win = tmp;

	*regsel = REDTBL + 2 * n;
	tmp = *win;
	tmp = (tmp & 0xfffea000) | ((vect | flags) & 0x00015fff);
	*win = tmp;
	lock.Exit();
}

void IOApic::MaskIRQ(unsigned char n)
{
	lock.Enter();
	*regsel = REDTBL + 2 * n;
	*win |= INT_MASK;
	lock.Exit();
}

void IOApic::UnmaskIRQ(unsigned char n)
{
	lock.Enter();
	*regsel = REDTBL + 2 * n;
	*win &= ~INT_MASK;
	lock.Exit();
}