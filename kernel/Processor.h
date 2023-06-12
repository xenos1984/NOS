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

		Processor* prev;
		Processor* next;
		State state;
		Thread* current;
		ProcessorData data;

		constexpr Processor(State s, ProcessorData d) : state{s}, data{d} {};
		bool Startup(void);

		static Processor* bsp;
	};
}

#endif
