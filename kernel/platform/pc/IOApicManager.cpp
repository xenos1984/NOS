// IOApicManager.cpp - Interrupt manager for IOApic based systems.

#include <new>
#include INC_PLATFORM(IOApicManager.h)
#include INC_PLATFORM(SMP.h)
#include INC_PLATFORM(ACPI.h)
#include INC_ARCH(Port.h)
#include INC_ARCH(Apic.h)
#include INC_BITS(PMode.h)
#include <Console.h>

using namespace Kernel;

SECTION(".init.text") IOApicManager::IOApicManager(IOApic* io, unsigned int n, unsigned int irq) : IRQManager(irq)
{
	ioapic = io;
	numioapics = n;

	Console::WriteMessage(Console::Style::OK, "Interrupts:", "Enabled %d IRQs on %d IOAPICs.", irq, n);
}

#ifdef CONFIG_SMP
void SECTION(".init.text") IOApicManager::InitSmp(void)
{
	unsigned int i, m, n;
	IOApic* io;

	n = SMP::GetIOApicCount();
	io = (IOApic*) new char[n * sizeof(IOApic)];

	Port::WriteU8(0x21, 0xff);
	Port::WriteU8(0xa1, 0xff);

	if(SMP::HasIMCR())
	{
		Port::WriteU8(0x22, 0x70);
		Port::WriteU8(0x23, 0x01);
	}

	m = 0;
	for(i = 0; i < n; i++)
	{
		new (&io[i]) IOApic(SMP::GetIOApic(i)->Address, m);
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

	if((n = ACPI::GetIOApicCount()) == 0)
		return;
	io = (IOApic*) new char[n * sizeof(IOApic)];

	Port::WriteU8(0x21, 0xff);
	Port::WriteU8(0xa1, 0xff);

	m = 0;
	for(i = 0; i < n; i++)
	{
		new (&io[i]) IOApic(ACPI::GetIOApic(i)->Address, ACPI::GetIOApic(i)->InterruptBase);
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
	Apic::SignalEOI();
}
