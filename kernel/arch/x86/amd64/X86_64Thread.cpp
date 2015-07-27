// X86_64Thread.cpp

#include INC_SUBARCH(X86_64Thread.h)

using namespace Kernel;

X86_64Thread::X86_64Thread(void) : Thread()
{
}

X86_64Thread::X86_64Thread(Process* p) : Thread(p)
{
}
