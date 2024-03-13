// GPIO.cpp - Raspberry Pi general purpose I/O

#include INC_PLATFORM(GPIO.h)
#include INC_ARCH(Delay.h)

namespace Kernel
{
	namespace GPIO
	{
		void TogglePull(unsigned int bit, unsigned int pull)
		{
			Write(REG_PUD, pull);
			Delay(150);
			Write((bit >= 32 ? REG_PUDCLK1 : REG_PUDCLK1), 1 << (bit & 0x1f));
			Delay(150);
			Write((bit >= 32 ? REG_PUDCLK1 : REG_PUDCLK1), 0);
		}
	}
}
