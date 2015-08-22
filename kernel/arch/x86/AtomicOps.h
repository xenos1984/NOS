// AtomicOps.h - Atomic operations in a multiprocessing environment.

#ifndef __ARCH_X86_ATOMICOPS_H__
#define __ARCH_X86_ATOMICOPS_H__

#if defined CONFIG_SMP || defined CONFIG_ACPI
#define LOCK "lock "
#else
#define LOCK
#endif

namespace Kernel
{
	inline AtomicLock::AtomicLock(void)
	{
		key = 0;
	}

	inline void AtomicLock::Enter(void)
	{
		asm volatile ("1: \n"
			"pause \n"
			LOCK "btsl $0, %[key] \n"
			"jc 1b" : [key] "+m" (key));
	}

	inline void AtomicLock::Exit(void)
	{
		asm volatile ("mfence\n"
			"movl $0, %[key]" : [key] "=m" (key));
	}
}

#undef LOCK

#endif
