// PIT.h - Support for programmable interval timer (Intel 8253).

#ifndef __PLATFORM_PC_PIT_H__
#define __PLATFORM_PC_PIT_H__

#include <SpinLock.h>
#include INC_ARCH(Port.h)

namespace Kernel
{
	/**
	 * Programmable interval timer.
	 * The PIT can be used for periodic interrupts, memory refresh and the pc speaker. It is accessed via registers 0x40 - 0x43.
	 */
	namespace PIT
	{
		enum PortAddr : uint8_t
		{
			baseaddr = 0x40,
			control = 0x43
		};

		enum TimerMode : uint8_t
		{
			MODE_BINARY      = 0x00,
			MODE_BCD         = 0x01,

			MODE_TERM_COUNT  = 0x00,
			MODE_ONE_SHOT    = 0x02,
			MODE_RATE        = 0x04,
			MODE_SQUARE      = 0x06,
			MODE_SOFT_STROBE = 0x08,
			MODE_HARD_STROBE = 0x0a,

			MODE_LATCHING    = 0x00,
			MODE_LSB_ONLY    = 0x10,
			MODE_MSB_ONLY    = 0x20,
			MODE_LSB_MSB     = 0x30
		};

		void Init(void);

		void SetPeriodic(uint8_t timer, uint16_t count);
		void SetSquareWave(uint8_t timer, uint16_t count);
		void SetOneShot(uint8_t timer, uint16_t count);
		uint16_t GetCurrent(uint8_t timer);

		static const unsigned int Frequency = 0x1234dd;
	}
}

#endif
