// Kernel.h - Kernel calls.

#ifndef __ARCH_X86_X32_KERNEL_H__
#define __ARCH_X86_X32_KERNEL_H__

extern "C"
{
	/**
	 * CreateThread
	 * Create a new thread within the current process.
	 * @param ip Initial instruction pointer.
	 * @param stack Initial stack.
	 */
	void CreateThread(void* ip, void* stack) __attribute__((fastcall));

	/**
	 * ExitThread
	 * Exit current thread.
	 */
	void ExitThread(void) __attribute__((fastcall, noreturn));

	/**
	 * CreateProcess
	 * Create a new process from an existing ELF image.
	 * @param elf Path of an ELF image.
	 * @param cmd Command line to pass to the new process.
	 */
	void CreateProcess(const char* elf, const char* cmd);

	/**
	 * ExitProcess
	 * Exit current process.
	 */
	void ExitProcess(void) __attribute__((fastcall, noreturn));
}

#endif
