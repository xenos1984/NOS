// IRQ.h - Architecture independent part of IRQ manager.

#ifndef __IRQ_H__
#define __IRQ_H__

#include <Thread.h>

namespace Kernel
{
	namespace IRQ
	{
		void Mask(unsigned int irq);
		void Unmask(unsigned int irq);
		void SendEOI(unsigned int irq);
	};
}

#endif
