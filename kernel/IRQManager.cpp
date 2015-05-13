// IRQManager.cpp - Architecture independent part of IRQ manager.

#include <IRQManager.h>

using namespace Kernel;

SECTION(".init.text") IRQManager::IRQManager(unsigned int count)
{
	handler = new Thread*[count];
}

IRQManager::~IRQManager(void)
{
}

void IRQManager::HandleInterrupt(unsigned int irq)
{
	SendEOI(irq);
}
