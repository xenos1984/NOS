// FloppyDriver.h - Floppy driver.

#ifndef __FLOPPYDRIVER_H__
#define __FLOPPYDRIVER_H__

#include <Driver.h>

namespace Drivers
{
	class FloppyDriver : public Driver
	{
	private:
		static const unsigned short primary   = 0x3f0;
		static const unsigned short secondary = 0x370;

		static const unsigned short REG_STATUS_A    = 0x00;
		static const unsigned short REG_STATUS_B    = 0x01;
		static const unsigned short REG_DOR         = 0x02;
		static const unsigned short REG_STATUS_MAIN = 0x04;
		static const unsigned short REG_DSR         = 0x04;
		static const unsigned short REG_DATA        = 0x05;
		static const unsigned short REG_DIR         = 0x07;
		static const unsigned short REG_CCR         = 0x07;

	public:
		FloppyDriver(char* cmd = NULL);

		static void TestFunc(void);
	};
}

#endif
