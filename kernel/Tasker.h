// Tasker.h - Task management.

#ifndef __TASKER_H__
#define __TASKER_H__

#include <Thread.h>
#include <Process.h>
#include <Elf.h>

namespace Kernel
{

	namespace Tasker
	{
		int GetCurrentCPU(void); /**< Which CPU is this code running at? */
		Process* CreateProcess(Elf* elf, const char* cmd); /**< Creates a new process. */
		Thread* CreateThread(Process* p, uintptr_t entry); /**< Creates a new thread. */
		void DeleteThread(Thread* t); /**< Deletes a thread. */
		void DeleteProcess(Process* p); /**< Deletes a process. */
		void SwitchTo(Thread* from, Thread* to); /**< Switch from one thread context to another. */
	}
}
#endif
