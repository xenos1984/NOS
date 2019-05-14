// PITClock.h - System clock based on PIT

#ifndef __VENDOR_PC_PITCLOCK_H__
#define __VENDOR_PC_PITCLOCK_H__

#include <Clock.h>

namespace Kernel
{
	class PITClock : public Clock
	{
	public:
		const int irq;

		PITClock(Time t0, int i);
		Time GetTime(void);
	};
}

inline Kernel::PITClock& pitclock(void) { return(reinterpret_cast<Kernel::PITClock&>(sysclock_space)); }

#endif
