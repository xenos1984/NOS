// X86Process.cpp - Processes for x86 architecture.

#include INC_ARCH(X86Process.h)

using namespace Kernel;

char kprocess_space[sizeof(X86Process)];

X86Process::X86Process(void) : Process()
{
}
