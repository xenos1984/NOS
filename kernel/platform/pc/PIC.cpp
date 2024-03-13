// PIC.cpp - Programmable Interrupt Controller

#include <cstddef>
#include INC_PLATFORM(PIC.h)
#include <Console.h>

using namespace Kernel;

void PIC::SetMaster(uint8_t base)
{
	Console::WriteMessage(Console::Style::OK, "Master PIC:", "mapped to 0x%2x", base);
	control.WriteU8(CTRL_ICW1 | ICW1_ICW4 | ICW1_CASCADE | ICW1_EDGE);
	mask.WriteU8(base);
	mask.WriteU8(0x01 << 0x02);
	mask.WriteU8(ICW4_8086 | ICW4_EOI_NORMAL | ICW4_NONBUF);
	mask.WriteU8(0xff);
	master = nullptr;
}

void PIC::SetSlave(uint8_t base, PIC* m)
{
	Console::WriteMessage(Console::Style::OK, "Slave PIC:", "mapped to 0x%2x", base);
	control.WriteU8(CTRL_ICW1 | ICW1_ICW4 | ICW1_CASCADE | ICW1_EDGE);
	mask.WriteU8(base);
	mask.WriteU8(0x02);
	mask.WriteU8(ICW4_8086 | ICW4_EOI_NORMAL | ICW4_NONBUF);
	mask.WriteU8(0xff);
	master = m;
}

void PIC::MaskIRQ(uint8_t irq)
{
	mask.WriteU8(mask.ReadU8() | (0x01 << irq));
}

void PIC::UnmaskIRQ(uint8_t irq)
{
	mask.WriteU8(mask.ReadU8() & ~(0x01 << irq));
}

void PIC::Notify(void)
{
	control.WriteU8(CTRL_OCW2 | OCW2_EOI_GEN);
	if(master != nullptr)
		master->Notify();
}

PIC PIC::Master{0x20};
PIC PIC::Slave{0xa0};
