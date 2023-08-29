// Process.h - Processes for amd64 architecture.

#ifndef __ARCH_X86_AMD64_PROCESS_H__
#define __ARCH_X86_AMD64_PROCESS_H__

#include INC_ARCH(Pager.h)

namespace Kernel
{
	struct ProcessData
	{
		Pager::PageTableLevel<0>* pgtab;
		uint64_t cr3;
	} PACKED;
}

#endif
