// PIC.h - Programmable Interrupt Controller

#ifndef __VENDOR_PC_PIC_H__
#define __VENDOR_PC_PIC_H__

#include <cstdint>
#include INC_ARCH(Port.h)

namespace Kernel
{
	class PIC
	{
	private:
		Port control, mask;
		PIC* master;

	public:
		PIC(uint16_t addr);

		void SetMaster(uint8_t base);
		void SetSlave(uint8_t base, PIC* m);

		void MaskIRQ(uint8_t irq);
		void UnmaskIRQ(uint8_t irq);

		void Notify(void);
	};
}

#endif
