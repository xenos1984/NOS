// DescriptorTable.cpp - Handling descriptor tables

#include INC_ARCH(DescriptorTable.h)
#include INC_SUBARCH(Entry.h)
#include INC_ARCH(Interrupt.h)
#include <Console.h>

using namespace Kernel;

extern "C" void sysentry(void);

SECTION(".init.text") DescriptorTable::DescriptorTable(void)
{
}

SECTION(".init.text") void DescriptorTable::Clear(int sel)
{
#ifdef ELF64
	if((raw[sel] & 0x0000800000000000) && !(raw[sel] & 0x0000100000000000))
		raw[sel + 1] = 0;
#endif
	raw[sel] = 0;
}

SECTION(".init.text") void DescriptorTable::CreateInterruptGate(int n, uint16_t cs, void (*ip)(void), uint8_t dpl)
{
#ifdef ELF64
	gate[n].OffsetHigh = (unsigned long)ip >> 32;
#endif
	gate[n].OffsetMed = (unsigned long)ip >> 16;
	gate[n].OffsetLow = (unsigned long)ip;
	gate[n].Selector = cs;
	gate[n].Type = D_INT386;
	gate[n].Present = 1;
	gate[n].Privilege = dpl;
}

SECTION(".init.text") void DescriptorTable::CreateTrapGate(int n, uint16_t cs, void (*ip)(void), uint8_t dpl)
{
#ifdef ELF64
	gate[n].OffsetHigh = (unsigned long)ip >> 32;
#endif
	gate[n].OffsetMed = (unsigned long)ip >> 16;
	gate[n].OffsetLow = (unsigned long)ip;
	gate[n].Selector = cs;
	gate[n].Type = D_TRAP386;
	gate[n].Present = 1;
	gate[n].Privilege = dpl;
}

SECTION(".init.text") void DescriptorTable::CreateTSS(int n, void* tss, uint32_t length)
{
#ifdef ELF64
	system[n].BaseHigh2 = (unsigned long)tss >> 32;
#endif
	system[n].BaseHigh = (unsigned long)tss >> 24;
	system[n].BaseMed = (unsigned long)tss >> 16;
	system[n].BaseLow = (unsigned long)tss;
	system[n].LimitHigh = length >> 16;
	system[n].LimitLow = length;
	system[n].Type = D_TSSA386;
	system[n].Present = 1;
	system[n].Granularity = 0;
}

SECTION(".init.text") void DescriptorTable::InitIDT(void)
{
	CreateTrapGate(0x00, KERNEL_CODE, &DivideErrorWrapper);
	CreateTrapGate(0x01, KERNEL_CODE, &DebugInterruptWrapper);
	CreateTrapGate(0x02, KERNEL_CODE, &NonMaskableWrapper);
	CreateTrapGate(0x03, KERNEL_CODE, &BreakPointWrapper);
	CreateTrapGate(0x04, KERNEL_CODE, &OverflowWrapper);
	CreateTrapGate(0x05, KERNEL_CODE, &ArrayboundsWrapper);
	CreateTrapGate(0x06, KERNEL_CODE, &InvalidOpcodeWrapper);
	CreateTrapGate(0x07, KERNEL_CODE, &DeviceNotAvailableWrapper);
	CreateTrapGate(0x08, KERNEL_CODE, &DoubleFaultWrapper);
	CreateTrapGate(0x09, KERNEL_CODE, &CoprocSegOverrunWrapper);
	CreateTrapGate(0x0a, KERNEL_CODE, &InvalidTSSWrapper);
	CreateTrapGate(0x0b, KERNEL_CODE, &SegmentNotPresentWrapper);
	CreateTrapGate(0x0c, KERNEL_CODE, &StackFaultWrapper);
	CreateTrapGate(0x0d, KERNEL_CODE, &GeneralProtectionWrapper);
	CreateTrapGate(0x0e, KERNEL_CODE, &PageFaultWrapper);
	CreateTrapGate(0x10, KERNEL_CODE, &CoprocErrorWrapper);
	CreateTrapGate(0x11, KERNEL_CODE, &AlignmentCheckWrapper);
	CreateTrapGate(0x12, KERNEL_CODE, &MachineCheckWrapper);
	CreateTrapGate(0x13, KERNEL_CODE, &SSEFaultWrapper);
	CreateInterruptGate(FIRST_IRQ + 7, KERNEL_CODE, &SpuriousIRQWrapper);
	CreateInterruptGate(0x40, KERNEL_CODE, &ApicTimerIRQWrapper);
	CreateTrapGate(0xf0, KERNEL_CODE, &sysentry, 3);
#if defined ELF32
	CreateInterruptGate(FIRST_IRQ + 8, KERNEL_CODE, &CMOSTimerIRQWrapper);
#endif
	console().WriteMessage(Console::MSG_OK, "IDT:", "Installed @ 0x%p", this);
}
