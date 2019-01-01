// Symbol.h - Memory locations used by the kernel.

#ifndef __ARCH_ARM_A64_SYMBOL_H__
#define __ARCH_ARM_A64_SYMBOL_H__

namespace Kernel
{
	namespace Symbol
	{
		static constexpr ConstSymbol programStart    = ConstSymbol(0x0000000000000000UL);
		static constexpr ConstSymbol programEnd      = ConstSymbol(0x0000007fffffffffUL);
		static constexpr ConstSymbol libraryStart    = ConstSymbol(0xffffff8000000000UL);
		static constexpr ConstSymbol libraryEnd      = ConstSymbol(0xfffffffeffffffffUL);
		static constexpr ConstSymbol supervisorStart = ConstSymbol(0xffffffff00000000UL);
		static constexpr ConstSymbol supervisorEnd   = ConstSymbol(0xffffffffffffffffUL);

		static constexpr ConstSymbol heapStart       = ConstSymbol(0xffffffff00000000UL);
		static constexpr ConstSymbol heapTab         = ConstSymbol(0xffffffff3c000000UL);
		static constexpr ConstSymbol heapEnd         = ConstSymbol(0xffffffff3fffffffUL);

		static constexpr ConstSymbol kernelOffset    = ConstSymbol(0xffffffffc0000000UL);
	}
}

#endif
