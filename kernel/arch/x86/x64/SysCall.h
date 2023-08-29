// SysCall.h - Wrapper code to call PL0 functions in AMD64 architecture.

#ifndef __ARCH_X86_X64_SYSCALL_H__
#define __ARCH_X86_X64_SYSCALL_H__

#define SYSCALL(name, args, count) \
void name args

#endif
