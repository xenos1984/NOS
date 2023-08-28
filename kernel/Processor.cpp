// Processor.cpp - abstract Processor::processor class.

#include <Processor.h>

namespace Kernel
{
#if CPU_COUNT == 0
	unsigned int Processor::nproc;
	Processor* Processor::proc;
#elif CPU_COUNT == 1
	constinit Processor Processor::proc;
#else
	constinit Processor Processor::proc[CPU_COUNT];
#endif
}
