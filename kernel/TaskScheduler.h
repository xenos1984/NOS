// TaskScheduler.h - Task scheduler class.

#ifndef __TASKSCHEDULER_H__
#define __TASKSCHEDULER_H__

#include <atomic>
#include <TaskManager.h>
#include <Thread.h>
#include <ThreadQueue.h>
#include <Process.h>
#include <SpinLock.h>

namespace Kernel
{
	class TaskScheduler
	{
	private:
		SpinLock lock;
		std::atomic<int> threadCount, processCount;
		Thread** current;
		ThreadQueue ready;

	public:
		TaskScheduler(unsigned long n);

		void CreateProcess(Process* p);
		void CreateThread(Thread* t);

		void DeleteProcess(Process* p);
		void DeleteThread(Thread* t);

		void ExitThread(void);
		void ExitProcess(void);

		inline Thread* GetCurrent(unsigned int n = taskman().GetCurrentCPU())
		{
			return(current[n]);
		}

		void Schedule(void);
		// void SwitchToIdle(void) __attribute__((noreturn));
	};
}

extern char tasksched_space[];
inline Kernel::TaskScheduler& tasksched(void) { return(reinterpret_cast<Kernel::TaskScheduler&>(tasksched_space)); }

#endif
