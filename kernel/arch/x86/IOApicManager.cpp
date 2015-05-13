// IOApicManager.cpp - Interrupt manager for IOApic based systems.

#include <new>
#include INC_ARCH(IOApicManager.h)
#include INC_ARCH(SMP.h)
#include INC_ARCH(ACPI.h)
#include INC_ARCH(Port.h)
#include INC_ARCH(Apic.h)
#include INC_SUBARCH(PMode.h)
#include <Console.h>

using namespace Kernel;

SECTION(".init.text") IOApicManager::IOApicManager(IOApic* io, unsigned int n, unsigned int irq) : IRQManager(irq)
{
	ioapic = io;
	numioapics = n;

	console().WriteMessage(Console::MSG_OK, "Interrupts:", "Enabled %d IRQs on %d IOAPICs.", irq, n);
}

#ifdef CONFIG_SMP
void SECTION(".init.text") IOApicManager::InitSmp(void)
{
	unsigned int i, m, n;
	IOApic* io;

	n = smp().GetIOApicCount();
	io = (IOApic*) new char[n * sizeof(IOApic)];

	Port::WriteU8(0x21, 0xff);
	Port::WriteU8(0xa1, 0xff);

	if(smp().HasIMCR())
	{
		Port::WriteU8(0x22, 0x70);
		Port::WriteU8(0x23, 0x01);
	}

	m = 0;
	for(i = 0; i < n; i++)
	{
		new (&io[i]) IOApic(smp().GetIOApic(i)->Address, m);
		m += io[i].GetMaxEntry() + 1;
	}

	new (irqman_space) IOApicManager(io, n, m);
}
#endif

#ifdef CONFIG_ACPI
void SECTION(".init.text") IOApicManager::InitAcpi(void)
{
	unsigned int i, m, n;
	IOApic* io;

	if((n = acpi().GetIOApicCount()) == 0)
		return;
	io = (IOApic*) new char[n * sizeof(IOApic)];

	Port::WriteU8(0x21, 0xff);
	Port::WriteU8(0xa1, 0xff);

	m = 0;
	for(i = 0; i < n; i++)
	{
		new (&io[i]) IOApic(acpi().GetIOApic(i)->Address, acpi().GetIOApic(i)->InterruptBase);
		m += io[i].GetMaxEntry() + 1;
	}

	new (irqman_space) IOApicManager(io, n, m);
}
#endif

void IOApicManager::Mask(unsigned int irq)
{
	unsigned int i;

	for(i = 0; i < numioapics; i++)
	{
		if((irq >= ioapic[i].GetBase()) && (irq < ioapic[i].GetBase() + ioapic[i].GetCount()))
		{
			ioapic[i].MaskIRQ(irq - ioapic[i].GetBase());
			return;
		}
	}
}

void IOApicManager::Unmask(unsigned int irq)
{
	unsigned int i;

	for(i = 0; i < numioapics; i++)
	{
		if((irq >= ioapic[i].GetBase()) && (irq < ioapic[i].GetBase() + ioapic[i].GetCount()))
		{
			ioapic[i].UnmaskIRQ(irq - ioapic[i].GetBase());
			return;
		}
	}
}

void IOApicManager::SendEOI(unsigned int irq __attribute__((unused)))
{
	apic().SignalEOI();
}
