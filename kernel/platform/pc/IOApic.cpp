// IOApic.cpp - I/O Apic support

#include INC_PLATFORM(IOApic.h)
#include <Console.h>
#include <Pager.h>
#include <Heap.h>

using namespace Kernel;

SECTION(".init.text") IOApic::IOApic(unsigned long addr, unsigned int base)
{
	unsigned int i;
	regsel = (volatile uint32_t*)Heap::Alloc(Memory::PGS_4K, Memory::PGS_4K, false);
	win = (volatile uint32_t*)(0x10 + (uintptr_t)regsel);
	Pager::MapPage<Memory::PGB_4K>(addr, (uintptr_t)regsel, Memory::MemType::KERNEL_RW);
	irqbase = base;
	irqcount = GetMaxEntry() + 1;

	for(i = 0; i < irqcount; i++)
	{
		if(irqbase + i < 16)
			SetIRQ(i, 0xff, irqbase + i + 0x20, INT_POL_HIGH | INT_EDGETRIG | INT_MASK | DEST_LOGICAL | DM_LOWPRI);
		else
			SetIRQ(i, 0xff, irqbase + i + 0x20, INT_POL_LOW | INT_LEVELTRIG | INT_MASK | DEST_LOGICAL | DM_LOWPRI);
	}

	Console::WriteMessage(Console::Style::INFO, "I/O APIC:", "at 0x%8x, Version 0x%2x, IRQ 0x%2x - 0x%2x", addr, GetVersion(), irqbase, irqbase + irqcount - 1);
}

unsigned int IOApic::GetID(void)
{
	unsigned int tmp;

	lock.Lock();
	*regsel = APICID;
	tmp = *win;
	lock.Unlock();
	return(tmp);
}

void IOApic::SetID(unsigned int id)
{
	lock.Lock();
	*regsel = APICID;
	*win = id;
	lock.Unlock();
}

unsigned char IOApic::GetVersion(void)
{
	unsigned char tmp;

	lock.Lock();
	*regsel = APICVER;
	tmp = *win & 0xff;
	lock.Unlock();
	return(tmp);
}

unsigned char IOApic::GetMaxEntry(void)
{
	unsigned char tmp;

	lock.Lock();
	*regsel = APICVER;
	tmp = (*win >> 16) & 0xff;
	lock.Unlock();
	return(tmp);
}

unsigned int IOApic::GetArbitration(void)
{
	unsigned int tmp;

	lock.Lock();
	*regsel = APICARB;
	tmp = *win;
	lock.Unlock();
	return(tmp);
}

void IOApic::SetIRQ(unsigned char n, unsigned char dest, unsigned char vect, unsigned int flags)
{
	unsigned int tmp;

	lock.Lock();
	*regsel = REDTBL + 2 * n + 1;
	tmp = *win;
	tmp = (tmp & 0x00ffffff) | ((unsigned int)dest << 24);
	*win = tmp;

	*regsel = REDTBL + 2 * n;
	tmp = *win;
	tmp = (tmp & 0xfffea000) | ((vect | flags) & 0x00015fff);
	*win = tmp;
	lock.Unlock();
}

void IOApic::MaskIRQ(unsigned char n)
{
	lock.Lock();
	*regsel = REDTBL + 2 * n;
	*win |= INT_MASK;
	lock.Unlock();
}

void IOApic::UnmaskIRQ(unsigned char n)
{
	lock.Lock();
	*regsel = REDTBL + 2 * n;
	*win &= ~INT_MASK;
	lock.Unlock();
}
