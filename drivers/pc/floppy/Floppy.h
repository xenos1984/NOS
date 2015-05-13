// Floppy.h - Floppy driver.

#ifndef __FLOPPY_H__
#define __FLOPPY_H__

#include <Driver.h>

namespace Drivers
{
	class Floppy : public Driver
	{
	public:
		Floppy(char* cmd = NULL);
	};
}

#endif
