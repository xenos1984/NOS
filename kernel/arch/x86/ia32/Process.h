// Process.h - Processes for i386 architecture.

#ifndef __ARCH_X86_IA32_PROCESS_H__
#define __ARCH_X86_IA32_PROCESS_H__

#include INC_ARCH(X86Pager.h)

namespace Kernel
{
	struct ProcessData
	{
		X86Pager::PageTable* pgtab;
		uint32_t cr3;
	} PACKED;
}

#endif
