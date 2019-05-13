// Processor.h - Processor management.

#ifndef __ARCH_X86_PROCESSOR_H__
#define __ARCH_X86_PROCESSOR_H__

#include INC_VENDOR(ACPI.h)
#include INC_VENDOR(SMP.h)
#include <Thread.h>

namespace Kernel
{
	/** Single x86 CPU (maybe in a multi-processor environment).
	 */
	class Processor
	{
	private:
		static const unsigned int AP_INIT_ADDR  = 0x00003000;

		unsigned char physID;
		unsigned char logID;
		unsigned int state;

	public:
		static Processor* bsp;
		static Processor* procs;
		static unsigned int count;

		Processor(void); /**< Create "default" processor by testing the cpu this code is running at. */

#ifdef CONFIG_ACPI
		Processor(ACPI::LApic* ala); /**< Create processor from ACPI data. */
#endif
#ifdef CONFIG_SMP
		Processor(SMP::Cpu* sc); /**< Create processor from SMP data. */
#endif

		~Processor(void);

#if defined CONFIG_SMP || defined CONFIG_ACPI
		void Startup(unsigned long stack); /**< Put processor in a wait loop, ready for executing. */
		void Shutdown(void); /**< Shut down processor. */
#endif

		/** Physical CPU ID. */
		inline unsigned char GetPhysID(void) const
		{
			return(physID);
		}

		/** Logical CPU ID. */
		inline unsigned char GetLogID(void) const
		{
			return(logID);
		}
	};
}

#endif
