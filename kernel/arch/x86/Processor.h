// Processor.h - Processor management.

#ifndef __ARCH_X86_PROCESSOR_H__
#define __ARCH_X86_PROCESSOR_H__

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

#if defined CONFIG_SMP || defined CONFIG_ACPI
		Processor(unsigned char pid); /**< Create processor with given local APIC physical ID. */

		void Startup(unsigned long stack); /**< Put processor in a wait loop, ready for executing. */
		void Shutdown(void); /**< Shut down processor. */
#endif

		~Processor(void);

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
