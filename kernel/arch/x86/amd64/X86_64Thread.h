// X86_64Thread.h - Threads for x86_64 architecture.

#ifndef __ARCH_X86_AMD64_X86_64THREAD_H__
#define __ARCH_X86_AMD64_X86_64THREAD_H__

#include <Thread.h>

namespace Kernel
{
	class X86_64Thread : public Thread
	{
	public:
		unsigned long rsp;
		unsigned long rsp0;
		unsigned char mmstate[] __attribute__ ((aligned(16)));

		X86_64Thread(void);
		X86_64Thread(Process* p);
	};
}

#endif
