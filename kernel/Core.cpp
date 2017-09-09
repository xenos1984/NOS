// Core.cpp - Entry point of the kernel.

#include <Core.h>
#include <Console.h>

using namespace Kernel;

void Core::Welcome(void)
{
	Console::Clear();
	Console::SetStyle(Console::Style::INFO);
	Console::Write(PACKAGE_NAME);
	Console::SetStyle(Console::Style::TEXT);
	Console::WriteFormat(" version %s [%s, %s]\n\n", PACKAGE_VERSION, __DATE__, __TIME__);
}

extern "C"
{
	void __cxa_pure_virtual(void)
	{
		Console::WriteLine("Kernel Panic: Calling pure virtual method.");
	}
}
