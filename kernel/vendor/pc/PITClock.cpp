// PITClock.cpp - System clock based on PIT

#include INC_VENDOR(PITClock.h)
#include INC_VENDOR(PIT.h)
#include INC_ARCH(DescriptorTable.h)
#include INC_BITS(Entry.h)
#include INC_ARCH(Interrupt.h)
#include <Core.h>
#include <IRQManager.h>
#include <Console.h>

using namespace Kernel;

char sysclock_space[sizeof(PITClock)];

void PITIRQ(void)
{
	clock().Tick();
	irqman().SendEOI(pitclock().irq);
}

SECTION(".init.text") PITClock::PITClock(Time t0, int i) : Clock(t0, Time::Second(65536) / PIT::Frequency), irq(i)
{
	tabIDT.CreateInterruptGate(FIRST_IRQ + irq, KERNEL_CODE, &PITIRQWrapper);
	PIT::SetPeriodic(0, 0);
	irqman().Unmask(irq);
	Console::WriteMessage(Console::Style::OK, "Clock:", "PIT at IRQ #%d, offset = %ld, tick = %ld", irq, offset.raw, tick.raw);
}

Time PITClock::GetTime(void)
{
	return(offset + Time::Second(65536 - PIT::GetCurrent(0)) / PIT::Frequency);
}
