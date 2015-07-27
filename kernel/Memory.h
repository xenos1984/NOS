// Memory.h - Memory addresses defined in linker scripts or as constants.

#ifndef __MEMORY_H__
#define __MEMORY_H__

namespace Kernel
{
	namespace Memory
	{
		class Symbol;

		extern "C" const Symbol textStart;
		extern "C" const Symbol textEnd;
		extern "C" const Symbol dataStart;
		extern "C" const Symbol dataEnd;
		extern "C" const Symbol bssStart;
		extern "C" const Symbol bssEnd;
		extern "C" const Symbol initStart;
		extern "C" const Symbol initEnd;
		extern "C" const Symbol userStart;
		extern "C" const Symbol userEnd;
		extern "C" const Symbol kernelStart;
		extern "C" const Symbol kernelEnd;

		static const uintptr_t TextStart = (uintptr_t)&textStart;
		static const uintptr_t TextEnd = (uintptr_t)&textEnd;
		static const uintptr_t DataStart = (uintptr_t)&dataStart;
		static const uintptr_t DataEnd = (uintptr_t)&dataEnd;
		static const uintptr_t BssStart = (uintptr_t)&bssStart;
		static const uintptr_t BssEnd = (uintptr_t)&bssEnd;
		static const uintptr_t InitStart = (uintptr_t)&initStart;
		static const uintptr_t InitEnd = (uintptr_t)&initEnd;
		static const uintptr_t UserStart = (uintptr_t)&userStart;
		static const uintptr_t UserEnd = (uintptr_t)&userEnd;
		static const uintptr_t KernelStart = (uintptr_t)&kernelStart;
		static const uintptr_t KernelEnd = (uintptr_t)&kernelEnd;
	}
}

#include INC_ARCH(Memory.h)

#endif
