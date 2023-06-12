// Processor.h - Processor management.

#ifndef __ARCH_X86_PROCESSOR_H__
#define __ARCH_X86_PROCESSOR_H__

#include <cstdint>

namespace Kernel
{
	struct ProcessorData
	{
		uint32_t physID;
	};
	/*
	class Processor
	{
	private:
		static const unsigned int AP_INIT_ADDR  = 0x00003000;


	public:
		static Processor* bsp;
		static Processor* procs;
		static unsigned int count;

		Processor(void);

#if defined CONFIG_SMP || defined CONFIG_ACPI
		Processor(unsigned char pid);

#endif

		~Processor(void);

		inline unsigned char GetPhysID(void) const
		{
			return(physID);
		}

		inline unsigned char GetLogID(void) const
		{
			return(logID);
		}
	};*/
}

#endif
