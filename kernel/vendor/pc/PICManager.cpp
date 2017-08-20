// PICManager.cpp - Interrupt manager for PIC based systems.

#include INC_VENDOR(PICManager.h)
#include <Console.h>

using namespace Kernel;

char irqman_space[sizeof(PICManager)];

SECTION(".init.text") PICManager::PICManager(void) : IRQManager(16), master(0x20), slave(0xa0)
{
	master.SetMaster(0x20);
	slave.SetSlave(0x28, &master);
	master.UnmaskIRQ(2);
	Console::WriteMessage(Console::Style::OK, "Interrupts:", "Enabled in PIC mode.");
}

void PICManager::Mask(unsigned int irq)
{
	if(irq < 8)
		master.MaskIRQ(irq);
	else
		slave.MaskIRQ(irq - 8);
}

void PICManager::Unmask(unsigned int irq)
{
	if(irq < 8)
		master.UnmaskIRQ(irq);
	else
		slave.UnmaskIRQ(irq - 8);
}

void PICManager::SendEOI(unsigned int irq)
{
		master.Notify();
	if(irq >= 8)
		slave.Notify();
}
