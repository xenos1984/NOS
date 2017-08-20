// PIC.cpp - Programmable Interrupt Controller

#include <cstddef>
#include INC_VENDOR(PIC.h)

using namespace Kernel;

PIC::PIC(uint16_t addr) : control(addr), mask(addr + 1)
{
}

void PIC::SetMaster(uint8_t base)
{
	control.WriteU8(0x11);
	mask.WriteU8(base);
	mask.WriteU8(0x04);
	mask.WriteU8(0x01);
	mask.WriteU8(0xff);
	master = nullptr;
}

void PIC::SetSlave(uint8_t base, PIC* m)
{
	control.WriteU8(0x11);
	mask.WriteU8(base);
	mask.WriteU8(0x02);
	mask.WriteU8(0x01);
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
	control.WriteU8(0x20);
	if(master != nullptr)
		master->Notify();
}
