// TaskManager.h - Abstract task switcher class.

#ifndef __TASKMANAGER_H__
#define __TASKMANAGER_H__

#include <Thread.h>
#include <Process.h>
#include <Elf.h>

namespace Kernel
{

	class TaskManager
	{
	public:
		virtual int GetCurrentCPU(void) = 0; /**< Which CPU is this code running at? */
		virtual Process* CreateProcess(Elf* elf) = 0; /**< Creates a new process. */
		virtual Thread* CreateThread(Process* p, void* entry, void* stack) = 0; /**< Creates a new thread. */
		virtual void DeleteThread(Thread* t) = 0; /**< Deletes a thread. */
		virtual void DeleteProcess(Process* p) = 0; /**< Deletes a process. */
		virtual void SwitchTo(Thread* from, Thread* to) = 0; /**< Switch from one thread context to another. */
	};

}

extern char taskman_space[];
inline Kernel::TaskManager& taskman(void) { return(reinterpret_cast<Kernel::TaskManager&>(taskman_space)); }

#endif
