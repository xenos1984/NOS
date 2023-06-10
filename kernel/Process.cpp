// Process.cpp - Abstract process class.

#include <cstddef>
#include <Process.h>
#include <Thread.h>

using namespace Kernel;

char kprocess_space[sizeof(Process)];

Process::Process(const ProcessData& pd) : data(pd)
{
}

Process::Process(void)
{
	next = prev = this;
	state = ProcessStateActive;
	priority = 0;
	thread = nullptr;
}

Process::~Process(void)
{
}

void Process::AddThread(Thread* t)
{
	t->owner = this;
	threads++;
	lock.Lock();
	if(thread == nullptr) // Is this the first thread?
	{
		thread = t;
		t->nextP = t;
		t->prevP = t;
	}
	else
	{
		t->nextP = thread;
		t->prevP = thread->prevP;
		thread->prevP->nextP = t;
		thread->prevP = t;
	}
	lock.Unlock();
}

void Process::RemoveThread(Thread* t)
{
	if(t->owner != this)
		return;

	threads--;
	lock.Lock();
	if(t->nextP == t) // Is this the only thread?
		t->nextP = t->prevP = thread = nullptr;
	else
	{
		if(t == thread)
			thread = thread->nextP;
		t->nextP->prevP = t->prevP;
		t->prevP->nextP = t->nextP;
		t->nextP = t->prevP = nullptr;
	}
	lock.Unlock();
	t->owner = nullptr;
}
