// AtomicOps.h - Atomic operations in a multiprocessing environment.

#ifndef __ARCH_M68K_ATOMICOPS_H__
#define __ARCH_M68K_ATOMICOPS_H__

namespace Kernel
{
	inline AtomicLock::AtomicLock(void)
	{
		key = 0;
	}

	inline AtomicLock::~AtomicLock(void)
	{
	}

	inline void AtomicLock::Enter(void)
	{
		asm volatile ("1: \n"
			"bsetb #0, %[key] \n"
			"beqs 1b" : [key] "+m" (key));
	}

	inline void AtomicLock::Exit(void)
	{
		asm volatile ("movel #0, %[key]" : [key] "=m" (key));
	}
}

#endif
