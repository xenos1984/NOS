// Core.cpp - Entry point of the kernel.

#include <Core.h>
#include <Console.h>

using namespace Kernel;

void Core::Welcome(void)
{
	unsigned int i;

	Console::Clear();
	Console::SetStyle(Console::Style::INFO);
	Console::Write(PACKAGE_NAME);
	Console::SetStyle(Console::Style::TEXT);
	Console::WriteFormat(" version %s [%s, %s]\n", PACKAGE_VERSION, __DATE__, __TIME__);

	for(i = 0; i < Console::GetColumns(); i++)
		Console::Write((char)196);
}

extern "C"
{
	void __cxa_pure_virtual(void)
	{
		Console::WriteLine("Kernel Panic: Calling pure virtual method.");
	}
}
