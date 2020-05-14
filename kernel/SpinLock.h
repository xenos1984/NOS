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

		inline static void DefaultIdle(void);

	public:
		template<typename IdleFunc = void()> void Lock(IdleFunc idle);
		void Lock(void);
		bool Try(void);
		void Unlock(void);
	};
}

#endif
