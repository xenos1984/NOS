// ThreadQueue.cpp - Management for thread queues.

#include <cstddef>
#include <ThreadQueue.h>

using namespace Kernel;

ThreadQueue::ThreadQueue(void)
{
	int i;

	lock.Lock();
	for(i = 0; i < 256; i++)
		first[i] = nullptr;
	priority = 0;
	lock.Unlock();
}

ThreadQueue::~ThreadQueue(void)
{
}

void ThreadQueue::Store(Thread* t)
{
//	Console::WriteFormat("[%d] Store(0x%16lx)\n", taskman().GetCurrentCPU(), t);
	lock.Lock();
	if(first[t->priority] == nullptr)
	{
		first[t->priority] = t;
		t->prevQ = t->nextQ = t;
	}
	else
	{
		t->prevQ = first[t->priority]->prevQ;
		t->nextQ = first[t->priority];
		t->prevQ->nextQ = t->nextQ->prevQ = t;
	}
	// If necessary, raise priority.
	if(t->priority > priority)
		priority = t->priority;
	lock.Unlock();
}

Thread* ThreadQueue::Retrieve(void)
{
	Thread* t = nullptr;

	lock.Lock();
	if(priority > 0)
		t = first[priority];
	if(t != nullptr)
	{
		if(t->nextQ == t)
		{
			first[priority] = nullptr;
			while((priority > 0) && (first[priority] == nullptr))
				priority--;
		}
		else
		{
			first[priority] = first[priority]->nextQ;
			t->prevQ->nextQ = t->nextQ;
			t->nextQ->prevQ = t->prevQ;
		}
		t->prevQ = t->nextQ = nullptr;
	}
	lock.Unlock();
//	Console::WriteFormat("[%d] Retrieve = 0x%16lx\n", taskman().GetCurrentCPU(), t);
	return(t);
}

Thread* ThreadQueue::Exchange(Thread* from)
{
	Thread* to = from;

//	Console::WriteFormat("[%d] Exchange(0x%16lx)\n", taskman().GetCurrentCPU(), from);
	lock.Lock();
	if(priority == from->priority)
	{
		// We have a thread with the same priority - get it and put the old thread into the queue.
		to = first[priority];
		to->prevQ->nextQ = from;
		to->nextQ->prevQ = from;
		from->nextQ = to->nextQ;
		from->prevQ = to->prevQ;
		first[priority] = from->nextQ;
	}
	else if(priority > from->priority)
	{
		// We have a thread with higher priority - save the old thread first.
		if(first[from->priority] == nullptr)
		{
			first[from->priority] = from;
			from->prevQ = from->nextQ = from;
		}
		else
		{
			from->prevQ = first[from->priority]->prevQ;
			from->nextQ = first[from->priority];
			from->prevQ->nextQ = from->nextQ->prevQ = from;
		}
		// Now get the new thread.
		to = first[priority];
		if(to->nextQ == to)
		{
			first[priority] = nullptr;
			while((priority > 0) && (first[priority] == nullptr))
				priority--;
		}
		else
		{
			first[priority] = first[priority]->nextQ;
			to->prevQ->nextQ = to->nextQ;
			to->nextQ->prevQ = to->prevQ;
		}
	}
	to->prevQ = to->nextQ = nullptr;
	lock.Unlock();
//	Console::WriteFormat("[%d] Exchange = 0x%16lx\n", taskman().GetCurrentCPU(), to);
	return(to);
}
