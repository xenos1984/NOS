// Symbol.h - Memory locations used by the kernel.

#ifndef __ARCH_X86_IA32_SYMBOL_H__
#define __ARCH_X86_IA32_SYMBOL_H__

namespace Kernel
{
	namespace Symbol
	{
		static constexpr ConstSymbol programStart    = ConstSymbol(0x00000000UL);
		static constexpr ConstSymbol programEnd      = ConstSymbol(0x7fffffffUL);
		static constexpr ConstSymbol libraryStart    = ConstSymbol(0x80000000UL);
		static constexpr ConstSymbol libraryEnd      = ConstSymbol(0xbfffffffUL);
		static constexpr ConstSymbol supervisorStart = ConstSymbol(0xc0000000UL);
		static constexpr ConstSymbol supervisorEnd   = ConstSymbol(0xffffffffUL);

		static constexpr ConstSymbol heapStart       = ConstSymbol(0xd0000000UL);
		static constexpr ConstSymbol heapTab         = ConstSymbol(0xdf000000UL);
		static constexpr ConstSymbol heapEnd         = ConstSymbol(0xdfffffffUL);

		static constexpr ConstSymbol kernelOffset    = ConstSymbol(0xc0000000UL);
	}
}

#endif
