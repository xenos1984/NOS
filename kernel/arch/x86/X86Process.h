// X86Process.h - Processes for x86 architecture.

#ifndef __ARCH_X86_X86PROCESS_H__
#define __ARCH_X86_X86PROCESS_H__

#include <Process.h>
#include INC_ARCH(X86Pager.h)

namespace Kernel
{
	class X86Process : public Process
	{
	public:
		X86Pager::PageTable* pgtab;
		unsigned long cr3;

		X86Process(void);
	};
}

/** Returns a reference to the kernel process. */
inline Kernel::X86Process& x86kprocess(void) { return(reinterpret_cast<Kernel::X86Process&>(kprocess_space)); }

#endif
