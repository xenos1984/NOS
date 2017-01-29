// FloppyDriver.cpp - Floppy driver.

#include <Kernel.h>
#include "FloppyDriver.h"

using namespace Drivers;

extern int stack;

FloppyDriver::FloppyDriver(char* cmd __attribute__((unused)))
{
	CreateThread((void*)&FloppyDriver::TestFunc, (void*)((unsigned long)&stack - 0x800));
/*
	asm volatile ("1:\n"
		"fld1\n"
		"fxch\n"
		"fsubp\n"
		"fmulp\n"
		"fldl2t\n"
		"fmulp\n"
		"fabs\n"
		"jmp 1b\n" : : "t"(0.4));
*/
	while(1);
}

void FloppyDriver::TestFunc(void)
{
	ExitProcess();
/*
	asm volatile ("1:\n"
		"fld1\n"
		"fxch\n"
		"fsubp\n"
		"fmulp\n"
		"fldpi\n"
		"fmulp\n"
		"fabs\n"
		"jmp 1b\n" : : "t"(0.6));
*/
	while(1);
}

FloppyDriver floppydriver;
