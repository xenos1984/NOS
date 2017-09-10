// Delay.h - Short delay by spinning CPU cycles.

#ifndef __ARCH_ARM_DELAY_H__
#define __ARCH_ARM_DELAY_H__

namespace Kernel
{
	inline void Delay(unsigned int count)
	{
		asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n" : "=r"(count) : [count]"0"(count) : "cc");
	}
}

#endif
