// User.cpp - User program syscall functions.

#include <api/Schedule.h>
#include INC_ARCH(SysCall.h)
#include <Console.h>
#include <TaskManager.h>
#include <TaskScheduler.h>

using namespace Kernel;

SYSCALL(CreateThread, (void* ip, void* stack), 2)
{
	taskman().CreateThread(tasksched().GetCurrent()->owner, ip, stack);
}

SYSCALL(ExitThread, (void), 0)
{
	tasksched().ExitThread();
}

SYSCALL(ExitProcess, (void), 0)
{
	tasksched().ExitProcess();
}
