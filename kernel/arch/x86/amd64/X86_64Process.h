// X86_64Process.h - Processes for x86_64 architecture.

#ifndef __ARCH_X86_AMD64_X86_64PROCESS_H__
#define __ARCH_X86_AMD64_X86_64PROCESS_H__

#include <Process.h>
#include INC_ARCH(X86Pager.h)

namespace Kernel
{
	class X86_64Process : public Process
	{
	public:
		X86Pager::PageTable* pml4t;
		unsigned long cr3;

		X86_64Process(void);
	};
}

/** Returns a reference to the kernel process. */
inline Kernel::X86_64Process& x86_64kprocess(void) { return(reinterpret_cast<Kernel::X86_64Process&>(kprocess_space)); }

#endif
