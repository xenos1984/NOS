// Clock.cpp - Abstract clock class.

#include <Clock.h>

using namespace Kernel;

Clock::Clock(Time t0, Time dt) : offset(t0), tick(dt)
{
}

void Clock::Tick(void)
{
	offset += tick;
}

void Clock::Delay(Time time)
{
	Time target = time + GetTime();

	while(GetTime() < target) ;
}
