// X86_64Process.cpp - Processes for x86_64 architecture.

#include INC_SUBARCH(X86_64Process.h)

using namespace Kernel;

char kprocess_space[sizeof(X86_64Process)];

X86_64Process::X86_64Process(void) : Process()
{
}
