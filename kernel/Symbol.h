// Symbol.h - Memory addresses defined in linker scripts or as constants.

#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#include <cstdint>

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

			inline void* Ptr(void) const
			{
				return const_cast<void*>((void*)this);
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

			inline void* Ptr(void) const
			{
				return (void*)addr;
			}
		};

		template <class T> class ExtObject
		{
		public:
			inline constexpr uintptr_t Addr(void) const
			{
				return (uintptr_t)this;
			}

			inline T* Ptr(void) const
			{
				return (T*)this;
			}

			inline T* operator->(void) const
			{
				return (T*)this;
			}
		};

		template <class T> class ConstObject
		{
		private:
			const uintptr_t addr;

		public:
			constexpr ConstObject(uintptr_t a) : addr(a) {}

			inline constexpr uintptr_t Addr(void) const
			{
				return addr;
			}

			inline T* Ptr(void) const
			{
				return (T*)addr;
			}

			inline T* operator->(void) const
			{
				return (T*)addr;
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
		extern "C" const ExtSymbol initTextStart;
		extern "C" const ExtSymbol initTextEnd;
		extern "C" const ExtSymbol initDataStart;
		extern "C" const ExtSymbol initDataEnd;
		extern "C" const ExtSymbol userStart;
		extern "C" const ExtSymbol userEnd;
		extern "C" const ExtSymbol kernelStart;
		extern "C" const ExtSymbol kernelEnd;
		extern "C" const ExtSymbol scratchStart;
		extern "C" const ExtSymbol scratchEnd;
	}
}

#include INC_ARCH(Symbol.h)

#endif
