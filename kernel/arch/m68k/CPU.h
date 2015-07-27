// CPU.h - M68K CPU management.

#ifndef __ARCH_M68K_CPU_H__
#define __ARCH_M68K_CPU_H__

inline void Stop(unsigned short sr)
{
	asm volatile("stop %0" : : "i"(sr));
}

inline void SetUSP(unsigned long usp)
{
	asm volatile("movel %0, %%usp" : : "a"(usp));
}

inline unsigned long GetUSP(void)
{
	unsigned long usp;

	asm volatile("movel %%usp, %0" : "=a"(usp));
	return(usp);
}

inline void Reset(void)
{
	asm volatile("reset");
}

#endif
