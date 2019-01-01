// Symbol.h - Memory locations used by the kernel.

#ifndef __ARCH_ARM_SYMBOL_H__
#define __ARCH_ARM_SYMBOL_H__

namespace Kernel
{
	namespace Symbol
	{
		extern "C" const ExtSymbol ExceptionVectorBase;
	}
}

#include INC_BITS(Symbol.h)

#endif
