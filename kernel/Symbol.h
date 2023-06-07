// Symbol.h - Memory addresses defined in linker scripts or as constants.

#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#include <cstdint>
#include <cstddef>

namespace Kernel
{
	namespace Symbol
	{
		class ExtSymbol
		{
		public:
			inline uintptr_t Addr(void) const
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

		template<class T> class ExtObject
		{
		private:
			T t;

		public:
			inline constexpr T* operator->()
			{
				return &t;
			}

			inline constexpr T* operator&()
			{
				return &t;
			}
		};

		template<class T, uintptr_t addr> class ConstObject
		{
		public:
			inline constexpr T* operator->()
			{
				return reinterpret_cast<T*>(addr);
			}
		};

		template<class T, size_t s> class ExtArray
		{
		private:
			T t[s];

		public:
			T& operator[](size_t i)
			{
				return t[i];
			}
		};

		template<class T, size_t s, uintptr_t addr> class ConstArray
		{
		public:
			T& operator[](size_t i)
			{
				return (reinterpret_cast<T*>(addr))[i];
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
