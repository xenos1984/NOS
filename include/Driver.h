// Driver.h - Driver class prototype.

#ifndef __DRIVER_H__
#define __DRIVER_H__

#include <cstddef>

namespace Drivers
{
	class Driver
	{
	public:
		Driver(char* cmd = NULL);

	protected:
		char* command;
	};
}

#endif
