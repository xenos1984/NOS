// I386Thread.h - Threads for i386 architecture.

#ifndef __ARCH_X86_IA32_I386THREAD_H__
#define __ARCH_X86_IA32_I386THREAD_H__

#include <Thread.h>

namespace Kernel
{
	class I386Thread : public Thread
	{
	public:
		unsigned long esp;
		unsigned long esp0;
		unsigned char mmstate[] __attribute__ ((aligned(16)));

		I386Thread(void);
		I386Thread(Process* p);
	};
}

#endif
