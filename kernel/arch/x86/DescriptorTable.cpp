// DescriptorTable.cpp - Handling descriptor tables

#include INC_ARCH(DescriptorTable.h)
#include INC_BITS(Entry.h)
#include INC_ARCH(Interrupt.h)
#include <Console.h>

using namespace Kernel;

extern "C" void sysentry(void);

inline DescriptorTable::Segment& DescriptorTable::Seg(int n)
{
	return reinterpret_cast<DescriptorTable::Segment*>(this)[n];
}

inline DescriptorTable::System& DescriptorTable::Sys(int n)
{
	return reinterpret_cast<DescriptorTable::System*>(this)[n];
}

inline DescriptorTable::Gate& DescriptorTable::Gat(int n)
{
	return reinterpret_cast<DescriptorTable::Gate*>(this)[n];
}

inline uint64_t& DescriptorTable::Raw(int n)
{
	return reinterpret_cast<uint64_t*>(this)[n];
}

SECTION(".init.text") void DescriptorTable::Clear(int sel)
{
	uint64_t& raw = Raw(sel);

#ifdef ELF64
	if((raw & 0x0000800000000000) && !(raw & 0x0000100000000000))
	{
		uint64_t& raw2 = Raw(sel + 1);
		raw2 = 0;
	}
#endif
	raw = 0;
}

SECTION(".init.text") void DescriptorTable::CreateInterruptGate(int sel, uint16_t cs, void (*ip)(void), uint8_t dpl)
{
	uintptr_t addr = (uintptr_t)ip;
	Gate& gate = Gat(sel);

#ifdef ELF64
	gate.OffsetHigh = addr >> 32;
#endif
	gate.OffsetMed = addr >> 16;
	gate.OffsetLow = addr;
	gate.Selector = cs;
	gate.Type = D_INT386;
	gate.Present = 1;
	gate.Privilege = dpl;
}

SECTION(".init.text") void DescriptorTable::CreateTrapGate(int sel, uint16_t cs, void (*ip)(void), uint8_t dpl)
{
	uintptr_t addr = (uintptr_t)ip;
	Gate& gate = Gat(sel);

#ifdef ELF64
	gate.OffsetHigh = addr >> 32;
#endif
	gate.OffsetMed = addr >> 16;
	gate.OffsetLow = addr;
	gate.Selector = cs;
	gate.Type = D_TRAP386;
	gate.Present = 1;
	gate.Privilege = dpl;
}

SECTION(".init.text") void DescriptorTable::CreateTSS(int sel, void* tss, uint32_t length)
{
	uintptr_t addr = (uintptr_t)tss;
	System& system = Sys(sel);

#ifdef ELF64
	system.BaseHigh2 = addr >> 32;
#endif
	system.BaseHigh = addr >> 24;
	system.BaseMed = addr >> 16;
	system.BaseLow = addr;
	system.LimitHigh = length >> 16;
	system.LimitLow = length;
	system.Type = D_TSSA386;
	system.Present = 1;
	system.Granularity = 0;
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
	Console::WriteMessage(Console::Style::OK, "IDT:", "Installed @ 0x%p", this);
}
