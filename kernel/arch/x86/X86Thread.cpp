// X86Thread.cpp

#include <cstddef>
#include INC_ARCH(X86Thread.h)

using namespace Kernel;

X86Thread::X86Thread(void) : Thread()
{
	mmstate = nullptr;
}

X86Thread::X86Thread(Process* p) : Thread(p)
{
	mmstate = nullptr;
}

X86Thread::~X86Thread(void)
{
	delete[] mmstate;
}
