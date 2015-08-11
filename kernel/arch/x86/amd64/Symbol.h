// Symbol.h - Memory locations used by the kernel.

#ifndef __ARCH_X86_AMD64_SYMBOL_H__
#define __ARCH_X86_AMD64_SYMBOL_H__

#include <cstdint>

namespace Kernel
{
	namespace Memory
	{
		static const ConstSymbol& programStart    = ConstSymbol(0x0000000000000000UL);
		static const ConstSymbol& programEnd      = ConstSymbol(0x00007fffffffffffUL);
		static const ConstSymbol& libraryStart    = ConstSymbol(0xffff800000000000UL);
		static const ConstSymbol& libraryEnd      = ConstSymbol(0xfffffeffffffffffUL);
		static const ConstSymbol& supervisorStart = ConstSymbol(0xffffff0000000000UL);
		static const ConstSymbol& supervisorEnd   = ConstSymbol(0xffffffffffffffffUL);

		static const ConstSymbol& heapStart       = ConstSymbol(0xfffffff000000000UL);
		static const ConstSymbol& heapTab         = ConstSymbol(0xfffffffe00000000UL);
		static const ConstSymbol& heapEnd         = ConstSymbol(0xfffffffeffffffffUL);

		static const ConstSymbol& kernelOffset    = ConstSymbol(0xffffffff80000000UL);
	}
}

#endif
