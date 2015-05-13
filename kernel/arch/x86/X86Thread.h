// X86Thread.h - Threads for x86 architecture.

#ifndef __ARCH_X86_X86THREAD_H__
#define __ARCH_X86_X86THREAD_H__

#include <Thread.h>

namespace Kernel
{
	class X86Thread : public Thread
	{
	public:
		unsigned long stack;
		unsigned long pl0stack;
		char* mmstate;

		X86Thread(void);
		X86Thread(Process* p);
		~X86Thread(void);
	};
}

#endif
