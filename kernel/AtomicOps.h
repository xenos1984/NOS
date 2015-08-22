// AtomicOps.h - Atomic operations in a multiprocessing environment.

#ifndef __ATOMICOPS_H__
#define __ATOMICOPS_H__

namespace Kernel
{
	class AtomicLock
	{
	private:
		volatile unsigned int key;

	public:
		AtomicLock(void);

		constexpr AtomicLock(unsigned int k) : key(k) {};

		void Enter(void);
		void Exit(void);
	};

	template<class T> class AtomicCounter
	{
	private:
		volatile T val;

	public:
		inline AtomicCounter(T n = 0)
		{
			val = n;
		}

		inline T Value(void)
		{
			return val;
		}

		inline T AddAndFetch(T n)
		{
			return __sync_add_and_fetch(&val, n);
		}

		inline T SubAndFetch(T n)
		{
			return __sync_sub_and_fetch(&val, n);
		}

		inline T FetchAndAdd(T n)
		{
			return __sync_fetch_and_add(&val, n);
		}

		inline T FetchAndSub(T n)
		{
			return __sync_fetch_and_sub(&val, n);
		}

		inline T IncAndFetch(void)
		{
			return AddAndFetch(1);
		}

		inline T DecAndFetch(void)
		{
			return SubAndFetch(1);
		}

		inline T FetchAndInc(void)
		{
			return FetchAndAdd(1);
		}

		inline T FetchAndDec(void)
		{
			return FetchAndSub(1);
		}
	};
}

#include INC_ARCH(AtomicOps.h)

#endif
