// Memory.h - Memory locations used by the kernel.

#ifndef __ARCH_X86_IA32_MEMORY_H__
#define __ARCH_X86_IA32_MEMORY_H__

namespace Kernel
{
	namespace Memory
	{
		static const ConstSymbol& programStart    = ConstSymbol(0x00000000UL);
		static const ConstSymbol& programEnd      = ConstSymbol(0x7fffffffUL);
		static const ConstSymbol& libraryStart    = ConstSymbol(0x80000000UL);
		static const ConstSymbol& libraryEnd      = ConstSymbol(0xbfffffffUL);
		static const ConstSymbol& supervisorStart = ConstSymbol(0xc0000000UL);
		static const ConstSymbol& supervisorEnd   = ConstSymbol(0xffffffffUL);

		static const ConstSymbol& heapStart       = ConstSymbol(0xd0000000UL);
		static const ConstSymbol& heapTab         = ConstSymbol(0xdf000000UL);
		static const ConstSymbol& heapEnd         = ConstSymbol(0xdfffffffUL);

		static const ConstSymbol& kernelOffset    = ConstSymbol(0xc0000000UL);
	}
}

#endif
