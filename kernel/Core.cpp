// Core.cpp - Entry point of the kernel.

#include <Core.h>
#include <Console.h>

using namespace Kernel;

void Core::Welcome(void)
{
	int i;

	console().Clear();
	console().SetForeground(Console::FG_WHITE);
	console().Write(PACKAGE_NAME);
	console().SetForeground(Console::FG_LIGHT_GRAY);
	console().WriteFormat(" version %s [%s, %s]\n", PACKAGE_VERSION, __DATE__, __TIME__);

	for(i = 0; i < console().GetColumns(); i++)
		console().Write((char) 196);
}

extern "C"
{
	void __cxa_pure_virtual(void)
	{
		console().WriteLine("Kernel Panic: Calling pure virtual method.");
	}
}
