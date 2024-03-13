// PICManager.h - Interrupt manager for PIC based systems.

#ifndef __PLATFORM_PC_PICMANAGER_H__
#define __PLATFORM_PC_PICMANAGER_H__

#include <IRQManager.h>
#include INC_PLATFORM(PIC.h)

namespace Kernel
{
	class PICManager : public IRQManager
	{
	private:
		PIC master, slave;

	public:
		PICManager(void);
		~PICManager(void);

		static void Init(void);

		void Mask(unsigned int irq);
		void Unmask(unsigned int irq);
		void SendEOI(unsigned int irq);
	};
}

#endif
