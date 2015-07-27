// I386Process.cpp - Processes for i386 architecture.

#include INC_SUBARCH(I386Process.h)

using namespace Kernel;

char kprocess_space[sizeof(I386Process)];

I386Process::I386Process(void) : Process()
{
}
