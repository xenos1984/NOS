// Interrupt.h - Interrupt handling

#ifndef __ARCH_X86_INTERRUPT_H__
#define __ARCH_X86_INTERRUPT_H__

#include <cstdint>
#include INC_SUBARCH(Registers.h)

#define INTERRUPT(name) \
extern "C" void User##name(void); \
extern "C" void Kernel##name(Kernel::RegisterSet* regs); \
extern "C" void name##Wrapper(void);

#define EXCEPTION(name) \
extern "C" void User##name(Kernel::SelectorErrorCode error); \
extern "C" void Kernel##name(Kernel::RegisterSet* regs, Kernel::SelectorErrorCode error); \
extern "C" void name##Wrapper(void);

#define PAGEFAULT(name) \
extern "C" void User##name(Kernel::PageFaultErrorCode error); \
extern "C" void Kernel##name(Kernel::RegisterSet* regs, Kernel::PageFaultErrorCode error); \
extern "C" void name##Wrapper(void);

#define TIMER(name) \
extern "C" void name(void); \
extern "C" void name##Wrapper(void);

INTERRUPT(DivideError)
INTERRUPT(DebugInterrupt)
INTERRUPT(NonMaskable)
INTERRUPT(BreakPoint)
INTERRUPT(Overflow)
INTERRUPT(Arraybounds)
INTERRUPT(InvalidOpcode)
INTERRUPT(DeviceNotAvailable)
EXCEPTION(DoubleFault)
INTERRUPT(CoprocSegOverrun)
EXCEPTION(InvalidTSS)
EXCEPTION(SegmentNotPresent)
EXCEPTION(StackFault)
EXCEPTION(GeneralProtection)
PAGEFAULT(PageFault)
INTERRUPT(CoprocError)
EXCEPTION(AlignmentCheck)
INTERRUPT(MachineCheck)
INTERRUPT(SSEFault)

TIMER(CMOSTimerIRQ)
TIMER(ApicTimerIRQ)
TIMER(PITIRQ)

extern "C" void SpuriousIRQWrapper(void);

#undef INTERRUPT
#undef EXCEPTION
#undef PAGEFAULT
#undef TIMER

#endif
