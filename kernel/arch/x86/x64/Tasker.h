// Tasker.h - Task management.

#ifndef __ARCH_X86_X64_TASKER_H__
#define __ARCH_X86_X64_TASKER_H__

#include INC_ARCH(Tasker.h)

namespace Kernel
{

	namespace Tasker
	{
		static const uintptr_t KernelStackAddr = 0xffffffff00000000;
		static const uintptr_t KernelStackSize = 0x2000;
	}
}
#endif
