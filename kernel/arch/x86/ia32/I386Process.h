// I386Process.h - Processes for i386 architecture.

#ifndef __ARCH_X86_IA32_I386PROCESS_H__
#define __ARCH_X86_IA32_I386PROCESS_H__

#include <Process.h>
#include INC_ARCH(X86Pager.h)

namespace Kernel
{
	class I386Process : public Process
	{
	public:
		X86Pager::PageTable* pgdir;
		unsigned long cr3;

		I386Process(void);
	};
}

/** Returns a reference to the kernel process. */
inline Kernel::I386Process& i386kprocess(void) { return(reinterpret_cast<Kernel::I386Process&>(kprocess_space)); }

#endif
