// PICManager.h - Interrupt manager for PIC based systems.

#ifndef __ARCH_X86_PICMANAGER_H__
#define __ARCH_X86_PICMANAGER_H__

#include <IRQManager.h>
#include INC_ARCH(PIC.h)

namespace Kernel
{
	class PICManager : public IRQManager
	{
	private:
		PIC master, slave;

	public:
		PICManager(void);
		~PICManager(void);

		void Mask(unsigned int irq);
		void Unmask(unsigned int irq);
		void SendEOI(unsigned int irq);
	};
}

#endif
