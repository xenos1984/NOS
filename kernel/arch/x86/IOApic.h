// IOApic.h - I/O Apic support

#ifndef __ARCH_X86_IOAPIC_H__
#define __ARCH_X86_IOAPIC_H__

#include <cstdint>
#include <AtomicOps.h>

namespace Kernel
{
	class IOApic
	{
	private:
		volatile uint32_t* regsel;
		volatile uint32_t* win;
		unsigned int irqbase, irqcount;
		AtomicLock lock;

		static const int APICID  = 0x00;
		static const int APICVER = 0x01;
		static const int APICARB = 0x02;
		static const int REDTBL  = 0x10;

	public:
		static const uint32_t INT_MASK      = 0x10000; /**< Interrupt masked */
		static const uint32_t INT_LEVELTRIG = 0x08000; /**< Level trigger mode */
		static const uint32_t INT_EDGETRIG  = 0x00000; /**< Edge trigger mode */
		static const uint32_t REMOTE_IRR    = 0x04000; /**< Interrupt accept */
		static const uint32_t INT_POL_LOW   = 0x02000; /**< Interrupt polarity low */
		static const uint32_t INT_POL_HIGH  = 0x00000; /**< Interrupt polarity high */
		static const uint32_t INT_PENDING   = 0x01000; /**< Interrupt pending */
		static const uint32_t DEST_LOGICAL  = 0x00800; /**< Logical address mode */
		static const uint32_t DEST_PHYSICAL = 0x00000; /**< Physical address mode */
		static const uint32_t DM_FIXED      = 0x00000; /**< Fixed */
		static const uint32_t DM_LOWPRI     = 0x00100; /**< Lowest priority */
		static const uint32_t DM_SMI        = 0x00200; /**< System management interrupt */
		static const uint32_t DM_NMI        = 0x00400; /**< Non-maskable interrupt */
		static const uint32_t DM_INIT       = 0x00500; /**< INIT */
		static const uint32_t DM_EXTINT     = 0x00700; /**< External interrupt */

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
