// IRQManager.h - Architecture independent part of IRQ manager.

#ifndef __IRQMANAGER_H__
#define __IRQMANAGER_H__

#include <Thread.h>

namespace Kernel
{
	class IRQManager
	{
	private:
		Thread** handler;

	public:
		IRQManager(unsigned int count);
		~IRQManager(void);

		void SetHandler(unsigned int irq, Thread* t);
		Thread* GetHandler(unsigned int irq);
		void DeleteHandler(unsigned int irq);
		void HandleInterrupt(unsigned int irq);

		virtual void Mask(unsigned int irq) = 0;
		virtual void Unmask(unsigned int irq) = 0;
		virtual void SendEOI(unsigned int irq) = 0;
	};
}

extern char irqman_space[];
inline Kernel::IRQManager& irqman(void) { return(reinterpret_cast<Kernel::IRQManager&>(irqman_space)); }

#endif
