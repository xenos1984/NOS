// TaskScheduler.cpp - Task scheduler class.

#include <cstddef>
#include <TaskScheduler.h>
#include <TaskManager.h>
#include <Console.h>
#include <Thread.h>
#include <Process.h>

using namespace Kernel;

char tasksched_space[sizeof(TaskScheduler)];

SECTION(".init.text") TaskScheduler::TaskScheduler(unsigned long n) : threadCount(n), processCount(1)
{
	unsigned long i;

	current = new Thread*[n];
	for(i = 0; i < n; i++)
		current[i] = nullptr;
}

void TaskScheduler::CreateThread(Thread* t)
{
	t->id = threadCount.IncAndFetch();
	t->priority = 255;
	ready.Store(t);
}

void TaskScheduler::CreateProcess(Process* p)
{
	lock.Enter();
	p->id = processCount.IncAndFetch();
	p->next = &kprocess();
	p->prev = kprocess().prev;
	kprocess().prev->next = p;
	kprocess().prev = p;
	lock.Exit();
}

void TaskScheduler::DeleteThread(Thread* t)
{
	Process* p;

	lock.Enter();
	p = t->owner;
	//console().WriteFormat("Delete thread @ 0x%8x, owner = 0x%8x, threads = %d\n", t, p, p->threads.Value());
	p->RemoveThread(t);
	if(p->threads.Value() == 0)
		DeleteProcess(p);
	taskman().DeleteThread(t);
	lock.Exit();
}

void TaskScheduler::DeleteProcess(Process* p)
{
	//console().WriteFormat("Delete process @ 0x%8x, prev = 0x%8x, next = 0x%8x\n", p, p->prev, p->next);
	p->next->prev = p->prev;
	p->prev->next = p->next;
	p->next = p->prev = nullptr;
	taskman().DeleteProcess(p);
}

void TaskScheduler::ExitThread(void)
{
	GetCurrent()->state = ThreadStateDeleted;
	Schedule();
}

void TaskScheduler::ExitProcess(void)
{
	GetCurrent()->owner->state = ProcessStateDeleted;
	Schedule();
}

void TaskScheduler::Schedule(void)
{
	int n = taskman().GetCurrentCPU();
	Thread* from = current[n];
	Thread* to;

	if(from != nullptr)
	{
		if((from->state == ThreadStateActive) && (from->owner->state == ProcessStateActive))
		{
			ready.Store(from);
		}
		else if((from->state == ThreadStateDeleted) || (from->owner->state == ProcessStateDeleted))
		{
			DeleteThread(from);
			from = nullptr;
		}
	}

	while(true)
	{
		to = ready.Retrieve();

		if(to == nullptr)
			break;

		if((to->state == ThreadStateReady) && (to->owner->state == ProcessStateActive))
		{
			to->state = ThreadStateActive;
			break;
		}

		if((to->state == ThreadStateDeleted) || (to->owner->state == ProcessStateDeleted))
			DeleteThread(to);
	}

	current[n] = to;
	taskman().SwitchTo(from, to);
}
/*
void TaskScheduler::SwitchToIdle(void)
{
	int n = taskman().GetCurrentCPU();
	Thread* from = current[n];
	Thread* to;

	to = idle[n];
	to->state = ThreadStateActive;
	current[n] = to;

//	console().WriteFormat("[%d] Switch from 0x%16lx to idle 0x%16lx.\n", n, from, to);
	taskman().SwitchTo(from, to);

	console().WriteMessage(Console::MSG_ERROR, "Kernel panic:", "Return to dead thread in SwitchToIdle.");
	while(true) ;
}
*/