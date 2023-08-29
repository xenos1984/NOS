// Process.h - Processes for i386 architecture.

#ifndef __ARCH_X86_X32_PROCESS_H__
#define __ARCH_X86_X32_PROCESS_H__

#include INC_ARCH(Pager.h)

namespace Kernel
{
	struct ProcessData
	{
		Pager::PageTableLevel<0>* pgtab;
		uint32_t cr3;
	} PACKED;
}

#endif
