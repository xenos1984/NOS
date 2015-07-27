// Process.h - Processes for amd64 architecture.

#ifndef __ARCH_X86_AMD64_PROCESS_H__
#define __ARCH_X86_AMD64_PROCESS_H__

#include INC_ARCH(X86Pager.h)

namespace Kernel
{
	struct ProcessData
	{
		X86Pager::PageTable* pgtab;
		uint64_t cr3;
	} PACKED;
}

#endif
