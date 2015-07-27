// IOApicManager.h - Interrupt manager for IOApic based systems.

#ifndef __ARCH_X86_IOAPICMANAGER_H__
#define __ARCH_X86_IOAPICMANAGER_H__

#include <IRQManager.h>
#include INC_ARCH(IOApic.h)

namespace Kernel
{
	class IOApicManager : public IRQManager
	{
	private:
		IOApic* ioapic;
		unsigned int numioapics;

		IOApicManager(IOApic* io, unsigned int n, unsigned int irq);

	public:
		~IOApicManager(void);

#ifdef CONFIG_SMP
		static void InitSmp(void);
#endif
#ifdef CONFIG_ACPI
		static void InitAcpi(void);
#endif

		void Mask(unsigned int irq);
		void Unmask(unsigned int irq);
		void SendEOI(unsigned int irq);
	};
}

#endif
