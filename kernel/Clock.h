// Clock.h - Abstract clock class.

#ifndef __CLOCK_H__
#define __CLOCK_H__

#include <Time.h>

namespace Kernel
{
	class Clock
	{
	protected:
		Time offset;
		const Time tick;

	public:
		Clock(Time t0, Time dt);
		~Clock(void);

		void Tick(void);
		void Delay(Time time);
		virtual Time GetTime(void) = 0;
	};
}

extern char sysclock_space[];
inline Kernel::Clock& clock(void) { return(reinterpret_cast<Kernel::Clock&>(sysclock_space)); }

#endif
