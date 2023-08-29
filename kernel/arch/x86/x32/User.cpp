// User.cpp - User program syscall functions.

#include <TaskManager.h>
#include <TaskScheduler.h>

using namespace Kernel;

extern "C"
{
	void CreateThread(void)
	{
		Thread* t = tasksched().GetCurrent();
		taskman().CreateThread(t->owner, (void*)t->data.ecx, (void*)t->data.edx);
	}

	void ExitThread(void)
	{
		tasksched().ExitThread();
	}

	void ExitProcess(void)
	{
		tasksched().ExitProcess();
	}
}
