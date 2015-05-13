// ThreadQueue.h - Management for thread queues.

#ifndef __THREADQUEUE_H__
#define __THREADQUEUE_H__

#include <Thread.h>
#include <AtomicOps.h>

namespace Kernel
{
	class ThreadQueue
	{
	private:
		Thread* first[256];
		AtomicLock lock;
		unsigned char priority;

	public:
		ThreadQueue(void);
		~ThreadQueue(void);

		void Store(Thread* t);
		Thread* Retrieve(void);
		Thread* Exchange(Thread* from);
	};
}

#endif
