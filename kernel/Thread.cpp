// Thread.cpp - Abstract thread class.

#include <cstddef>
#include <Thread.h>
#include <Process.h>
#include <Console.h>

using namespace Kernel;

Thread::Thread(void)
{
	owner = nullptr;
	prevP = nextP = prevQ = nextQ = nullptr;
	state = ThreadStateReady;
	priority = 0;
}

Thread::Thread(Process* p)
{
	prevQ = nextQ = nullptr;
	state = ThreadStateReady;
	priority = p->priority;
	p->AddThread(this);
}

Thread::~Thread(void)
{
}

void Thread::Dump(void)
{
	Console::WriteFormat("Thread #%d @ 0x%p; Process #%d @ 0x%p\n", id, this, owner->id, owner);
	data.Dump();
}
