// Memory.h - Memory addresses defined in linker scripts or as constants.

#ifndef __MEMORY_H__
#define __MEMORY_H__

namespace Kernel
{
	namespace Memory
	{
		class Symbol
		{
		public:
			inline constexpr uintptr_t Addr(void) const
			{
				return (uintptr_t)this;
			}

			inline const void* Pointer(void) const
			{
				return this;
			}
		};

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

		static const uintptr_t TextStart = textStart.Addr();
		static const uintptr_t TextEnd = textEnd.Addr();
		static const uintptr_t DataStart = dataStart.Addr();
		static const uintptr_t DataEnd = dataEnd.Addr();
		static const uintptr_t BssStart = bssStart.Addr();
		static const uintptr_t BssEnd = bssEnd.Addr();
		static const uintptr_t InitStart = initStart.Addr();
		static const uintptr_t InitEnd = initEnd.Addr();
		static const uintptr_t UserStart = userStart.Addr();
		static const uintptr_t UserEnd = userEnd.Addr();
		static const uintptr_t KernelStart = kernelStart.Addr();
		static const uintptr_t KernelEnd = kernelEnd.Addr();
	}
}

#include INC_ARCH(Memory.h)

#endif
