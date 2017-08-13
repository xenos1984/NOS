// Process.h - Abstract process class.

#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <cstdint>
#include <AtomicOps.h>
#include <Elf.h>
#include <Pager.h>
#include INC_ARCH(Process.h)

namespace Kernel
{
	enum ProcessState
	{
		ProcessStateActive,
		ProcessStateHalted,
		ProcessStateDeleted
	};

	class Thread;

	class Process
	{
	public:
		unsigned long id; /**< Unique ID to identify this process. */
		Process* parent; /**< Process which created this process. */
		Process* prev; /**< Previous process in list. */
		Process* next; /**< Next process in list. */
		ProcessState state; /**< Process state. */
		unsigned char priority; /**< Current priority used for new threads. */
		Thread* thread; /**< Pointer to first thread. */
		AtomicCounter<int> threads; /**< Number of threads. */
		AtomicCounter<unsigned long> memory; /**< Total memory occupied by this process. */
		AtomicCounter<uint64_t> time; /**< Total time consumed by this process. */
		AtomicLock lock; /**< Lock used to safely maintain the thread list. */

		ProcessData data; /**< Architecture specific data needed for each process. */
		Pager::Context context;

		Process(Elf* elf);
		Process(void);
		~Process(void);

		void AddThread(Thread* t);
		void RemoveThread(Thread* t);
	};
}

/** Space for kernel process. */
extern char kprocess_space[];

/** Returns a reference to the kernel process. */
inline Kernel::Process& kprocess(void) { return(reinterpret_cast<Kernel::Process&>(kprocess_space)); }

#endif
