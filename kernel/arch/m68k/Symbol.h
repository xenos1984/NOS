// Symbol.h - Memory locations used by the kernel.

#ifndef __ARCH_M68K_SYMBOL_H__
#define __ARCH_M68K_SYMBOL_H__

namespace Kernel
{
	namespace Symbol
	{
		static const ConstSymbol& heapStart       = ConstSymbol(0xd0000000UL);
		static const ConstSymbol& heapTab         = ConstSymbol(0xdf000000UL);
		static const ConstSymbol& heapEnd         = ConstSymbol(0xdfffffffUL);
	}
}

#endif
