// Symbol.h - Memory locations used by the kernel.

#ifndef __ARCH_X86_X64_SYMBOL_H__
#define __ARCH_X86_X64_SYMBOL_H__

#include <cstdint>

namespace Kernel
{
	namespace Symbol
	{
		static constexpr ConstSymbol programStart    = ConstSymbol(0x0000000000000000UL);
		static constexpr ConstSymbol programEnd      = ConstSymbol(0x00007fffffffffffUL);
		static constexpr ConstSymbol libraryStart    = ConstSymbol(0xffff800000000000UL);
		static constexpr ConstSymbol libraryEnd      = ConstSymbol(0xfffffeffffffffffUL);
		static constexpr ConstSymbol supervisorStart = ConstSymbol(0xffffff0000000000UL);
		static constexpr ConstSymbol supervisorEnd   = ConstSymbol(0xffffffffffffffffUL);

		static constexpr ConstSymbol heapStart       = ConstSymbol(0xfffffff000000000UL);
		static constexpr ConstSymbol heapTab         = ConstSymbol(0xfffffffe00000000UL);
		static constexpr ConstSymbol heapEnd         = ConstSymbol(0xfffffffeffffffffUL);

		static constexpr ConstSymbol kernelOffset    = ConstSymbol(0xffffffff80000000UL);

		extern "C" const ExtSymbol tabPGTAB;
		extern "C" const ExtSymbol tabPGDIR;
		extern "C" const ExtSymbol tabPGDIP;
		extern "C" const ExtSymbol tabPML4T;
	}
}

#endif
