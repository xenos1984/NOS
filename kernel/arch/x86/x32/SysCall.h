// SysCall.h - Wrapper code to call PL0 functions in IA32 architecture.

#ifndef __ARCH_X86_X32_SYSCALL_H__
#define __ARCH_X86_X32_SYSCALL_H__

#define SYSCALL(name, args, count) \
void name args

#endif
