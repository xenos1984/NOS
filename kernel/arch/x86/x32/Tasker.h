// Tasker.h - Task management.

#ifndef __ARCH_X86_X32_TASKER_H__
#define __ARCH_X86_X32_TASKER_H__

#include INC_ARCH(Tasker.h)

namespace Kernel
{

	namespace Tasker
	{
		static const uintptr_t KernelStackAddr = 0xf0000000;
		static const uintptr_t KernelStackSize = 0x1000;
	}
}
#endif
