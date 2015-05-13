// SysCall.h - Wrapper code to call supervisor functions in M68K architecture.

#ifndef __ARCH_M68K_SYSCALL_H__
#define __ARCH_M68K_SYSCALL_H__

#define SYSCALL(name, args, count) \
extern "C" SECTION(".user") void name args

#endif
