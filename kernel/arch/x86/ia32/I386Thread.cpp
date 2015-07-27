// I386Thread.cpp

#include INC_SUBARCH(I386Thread.h)

using namespace Kernel;

I386Thread::I386Thread(void) : Thread()
{
}

I386Thread::I386Thread(Process* p) : Thread(p)
{
}
