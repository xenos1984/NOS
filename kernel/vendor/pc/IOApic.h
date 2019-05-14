// IOApic.h - I/O Apic support

#ifndef __VENDOR_PC_IOAPIC_H__
#define __VENDOR_PC_IOAPIC_H__

#include <cstdint>
#include <SpinLock.h>

namespace Kernel
{
	class IOApic
	{
	private:
		volatile uint32_t* regsel;
		volatile uint32_t* win;
		unsigned int irqbase, irqcount;
		SpinLock lock;

		enum RegIndex : int
		{
			APICID  = 0x00,
			APICVER = 0x01,
			APICARB = 0x02,
			REDTBL  = 0x10
		};

	public:
		enum RegValue : uint32_t
		{
			INT_MASK      = 0x10000, /**< Interrupt masked */
			INT_LEVELTRIG = 0x08000, /**< Level trigger mode */
			INT_EDGETRIG  = 0x00000, /**< Edge trigger mode */
			REMOTE_IRR    = 0x04000, /**< Interrupt accept */
			INT_POL_LOW   = 0x02000, /**< Interrupt polarity low */
			INT_POL_HIGH  = 0x00000, /**< Interrupt polarity high */
			INT_PENDING   = 0x01000, /**< Interrupt pending */
			DEST_LOGICAL  = 0x00800, /**< Logical address mode */
			DEST_PHYSICAL = 0x00000, /**< Physical address mode */
			DM_FIXED      = 0x00000, /**< Fixed */
			DM_LOWPRI     = 0x00100, /**< Lowest priority */
			DM_SMI        = 0x00200, /**< System management interrupt */
			DM_NMI        = 0x00400, /**< Non-maskable interrupt */
			DM_INIT       = 0x00500, /**< INIT */
			DM_EXTINT     = 0x00700  /**< External interrupt */
		};

		IOApic(unsigned long addr, unsigned int base);

		unsigned int GetID(void);
		void SetID(unsigned int id);

		unsigned char GetVersion(void);
		unsigned char GetMaxEntry(void);
		unsigned int GetArbitration(void);

		inline unsigned int GetBase(void)
		{
			return(irqbase);
		}

		inline unsigned int GetCount(void)
		{
			return(irqcount);
		}

		void SetIRQ(unsigned char n, unsigned char dest, unsigned char vect, unsigned int flags);
		void MaskIRQ(unsigned char n);
		void UnmaskIRQ(unsigned char n);
	};
}

#endif
