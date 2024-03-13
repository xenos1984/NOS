// PIC.h - Programmable Interrupt Controller

#ifndef __PLATFORM_PC_PIC_H__
#define __PLATFORM_PC_PIC_H__

#include <cstdint>
#include INC_ARCH(Port.h)

namespace Kernel
{
	class PIC
	{
	private:
		Port control, mask;
		PIC* master;

		enum Ports : uint16_t
		{
			PORT_MASTER  = 0x20,
			PORT_SLAVE   = 0xa0,
			PORT_CONTROL = 0x00,
			PORT_MASK    = 0x01
		};

		enum Command : uint8_t
		{
			CTRL_ICW1        = 0x10,
			CTRL_OCW2        = 0x00,
			CTRL_OCW3        = 0x08,
			ICW1_ICW4        = 0x01,
			ICW1_SINGLE      = 0x02,
			ICW1_CASCADE     = 0x00,
			ICW1_ADI4        = 0x04,
			ICW1_ADI8        = 0x00,
			ICW1_LEVEL       = 0x08,
			ICW1_EDGE        = 0x00,
			ICW4_8086        = 0x01,
			ICW4_MCS80       = 0x00,
			ICW4_EOI_AUTO    = 0x02,
			ICW4_EOI_NORMAL  = 0x00,
			ICW4_NONBUF      = 0x00,
			ICW4_BUF_SLAVE   = 0x08,
			ICW4_BUF_MASTER  = 0x0c,
			ICW4_NESTED      = 0x10,
			OCW2_EOI_GEN     = 0x20,
			OCW2_EOI_SPEC    = 0x60,
			OCW2_ROT_GEN     = 0xa0,
			OCW2_ROT_SET     = 0x80,
			OCW2_ROT_CLEAR   = 0x00,
			OCW2_ROT_SPEC    = 0xe0,
			OCW2_SET_PRIO    = 0xc0,
			OCW2_NOP         = 0x40,
			OCW3_READ_ISR    = 0x03,
			OCW3_READ_IRR    = 0x02,
			OCW3_POLL        = 0x04,
			OCW3_SMASK_SET   = 0x60,
			OCW3_SMASK_RESET = 0x40
		};

	public:
		constexpr PIC(uint16_t addr) : control{addr}, mask{(uint16_t)(addr + 1)}, master{nullptr} {}

		void SetMaster(uint8_t base);
		void SetSlave(uint8_t base, PIC* m);

		void MaskIRQ(uint8_t irq);
		void UnmaskIRQ(uint8_t irq);

		void Notify(void);

		static PIC Master;
		static PIC Slave;
	};
}

#endif
