// Symbol.h - Memory addresses defined in linker scripts or as constants.

#ifndef __SYMBOL_H__
#define __SYMBOL_H__

namespace Kernel
{
	namespace Symbol
	{
		class ExtSymbol
		{
		public:
			inline constexpr uintptr_t Addr(void) const
			{
				return (uintptr_t)this;
			}

			inline const void* Ptr(void) const
			{
				return this;
			}
		};

		class ConstSymbol
		{
		private:
			const uintptr_t addr;

		public:
			constexpr ConstSymbol(uintptr_t a) : addr(a) {}

			inline constexpr uintptr_t Addr(void) const
			{
				return addr;
			}

			inline const void* Ptr(void) const
			{
				return (const void*)addr;
			}
		};

		extern "C" const ExtSymbol textStart;
		extern "C" const ExtSymbol textEnd;
		extern "C" const ExtSymbol dataStart;
		extern "C" const ExtSymbol dataEnd;
		extern "C" const ExtSymbol bssStart;
		extern "C" const ExtSymbol bssEnd;
		extern "C" const ExtSymbol initStart;
		extern "C" const ExtSymbol initEnd;
		extern "C" const ExtSymbol userStart;
		extern "C" const ExtSymbol userEnd;
		extern "C" const ExtSymbol kernelStart;
		extern "C" const ExtSymbol kernelEnd;
	}
}

#include INC_ARCH(Memory.h)

#endif
