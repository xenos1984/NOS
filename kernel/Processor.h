// Processor.h - abstract processor class.

#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__

#include INC_ARCH(Processor.h)

namespace Kernel
{
	class Thread;

	class Processor
	{
	public:
		enum class State
		{
			Offline,
			Booting,
			Online
		};

		State state;
		Thread* current;
		ProcessorData data;

		constexpr Processor(void) : state{State::Offline} {};
		constexpr Processor(State s, ProcessorData d) : state{s}, data{d} {};

#if CPU_COUNT == 0
		static unsigned int nproc;
		static Processor* proc;
#elif CPU_COUNT == 1
		static Processor proc;
#else
		static Processor proc[CPU_COUNT];
#endif
	};
}

#endif
