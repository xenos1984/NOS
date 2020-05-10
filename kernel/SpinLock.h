// SpinLock.h - Different types of spin locks.

#ifndef __SPINLOCK_H__
#define __SPINLOCK_H__

#include <atomic>

namespace Kernel
{
	class SpinLock
	{
	private:
#if ATOMIC_BOOL_LOCK_FREE == 2
		std::atomic_bool lock = false;
#else
		std::atomic_flag lock = ATOMIC_FLAG_INIT;
#endif

	public:
		template<typename IdleFunc = void()> void Lock(IdleFunc idle = []{});
		bool Try(void);
		void Unlock(void);
	};
}

#endif
