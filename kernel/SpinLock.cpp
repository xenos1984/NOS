// SpinLock.h - Different types of spin locks.

#include <SpinLock.h>

using namespace Kernel;

#if ATOMIC_BOOL_LOCK_FREE == 2
template<typename IdleFunc> void SpinLock::Lock(IdleFunc idle)
{
	while(true)
	{
		if(!lock.load(std::memory_order_acquire))
		{
			if(!lock.exchange(true, std::memory_order_acquire))
				return;
		}

		idle();
	}
}

bool SpinLock::Try(void)
{
	if(!lock.load(std::memory_order_acquire))
	{
		if(!lock.exchange(true, std::memory_order_acquire))
			return true;
	}

	return false;
}

void SpinLock::Unlock(void)
{
	lock.store(false, std::memory_order_release);
}
#else
template<typename IdleFunc> void SpinLock::Lock(IdleFunc idle)
{
	while(lock.test_and_set(std::memory_order_acquire))
		idle();
}

bool SpinLock::Try(void)
{
	return !lock.test_and_set(std::memory_order_acquire);
}

void SpinLock::Unlock(void)
{
	lock.clear(std::memory_order_release);
}
#endif

template void SpinLock::Lock<void()>(void());
