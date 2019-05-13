// Thread.h - Abstract thread class.

#ifndef __THREAD_H__
#define __THREAD_H__

#include <cstdint>
#include <atomic>
#include <SpinLock.h>
#include INC_ARCH(Thread.h)

namespace Kernel
{
	enum ThreadState
	{
		ThreadStateReady,
		ThreadStateActive,
		ThreadStateWaiting,
		ThreadStateHalted,
		ThreadStateDeleted
	};

	class Process;

	union ThreadID
	{
	public:
		struct
		{
#if defined ELF32
			uintX_t id : 18;
			uintX_t version : 14;
#elif defined ELF64
			uintX_t id : 32;
			uintX_t version : 32;
#else
#error "Invalid word size for ELF format."
#endif
		} idv;
		uintX_t raw;

		inline ThreadID(uintX_t i, uintX_t v) {idv.id = i; idv.version = v;};

		inline static const ThreadID Nil(void) {return ThreadID(0, 0);};
		inline static const ThreadID Any(void) {return ThreadID(-1, -1);};
		inline static const ThreadID AnyLocal(void) {return ThreadID(-1, 0);};
		inline static const ThreadID Interrupt(int i) {return ThreadID(i, -1);};
	};

	inline bool operator==(ThreadID t1, ThreadID t2) {return(t1.raw == t2.raw);}
	inline bool operator!=(ThreadID t1, ThreadID t2) {return(t1.raw != t2.raw);}

	class Thread
	{
	public:
		unsigned long id; /**< Unique ID to identify this thread. */
		Process* owner; /**< Owner process this thread belongs to. */
		Thread* prevP; /**< Previous thread within this process. */
		Thread* nextP; /**< Next thread within this process. */
		Thread* prevQ; /**< Previous thread in queue. */
		Thread* nextQ; /**< Next thread in queue. */
		ThreadState state; /**< Thread state. */
		unsigned char priority; /**< Current thread priority. */
		unsigned int slice; /**< Time slice for this thread. */
		unsigned int delta; /**< Delta value for wakeup queue. */
		std::atomic<uint64_t> total; /**< Total time consumed by this thread. */
		SpinLock lock; /**< Lock this thread when it is attached to a CPU. */

		ThreadData data; /**< Architecture specific data needed for each thread. */

		Thread(void);
		Thread(Process* p);
		~Thread(void);

		void Dump(void);
	};
}

#endif
