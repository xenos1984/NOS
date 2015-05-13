// Driver.cpp - Floppy driver.

#include <Driver.h>

using namespace Drivers;

Driver::Driver(char* cmd)
{
	command = cmd;
}
/*
extern "C"
{
	extern void (*Constructor)(char*);

	void start(char* cmd)
	{
		Constructor(cmd);
	}
}
*/
