// Chunker.h - Physical memory manager aka "chunker" function declarations.

#ifndef __CHUNKER_H__
#define __CHUNKER_H__

#include <bit>
#include <Memory.h>

namespace Kernel
{
	namespace Chunker
	{
		/** Initialize the chunker with the first area of available memory. */
		void Init(Memory::PhysAddr start, Memory::PhysAddr length, Memory::Zone zone);
		/** Add another region of available memory to the chunker. */
		void AddRegion(Memory::PhysAddr start, Memory::PhysAddr length, Memory::Zone zone);

		/** Find a free chunk of a given size and allocate it. */
		template<Memory::PageBits bits> Memory::PhysAddr Alloc(Memory::Zone zone = static_cast<Memory::Zone>(0));
		/** Mark a chunk of memory as free. */
		template<Memory::PageBits bits> bool Free(Memory::PhysAddr addr);
		/** Mark a chunk of memory as reserved. */
		template<Memory::PageBits bits> bool Reserve(Memory::PhysAddr addr);

		/** Mark a range of memory as free. */
		bool Free(Memory::PhysAddr first, Memory::PhysAddr last);
		/** Mark a range of memory as reserved. */
		bool Reserve(Memory::PhysAddr first, Memory::PhysAddr last);
	}
}

#include <chunker/bitmap/Chunker.h>

namespace Kernel
{
	namespace Chunker
	{
		/** Indicates whether pages of this size can be allocated / freed by the chunker. */
		constexpr bool IsValidSize(Memory::PageBits size)
		{
			return Memory::IsValidSize(size, ValidSizes);
		}

		static const Memory::PageBits MinPageBits = static_cast<Memory::PageBits>(std::countr_zero(ValidSizes));
		static const Memory::PageBits MaxPageBits = static_cast<Memory::PageBits>(std::bit_width(ValidSizes) - 1);

		static const Memory::PageSize MinPageSize = static_cast<Memory::PageSize>(1ULL << MinPageBits);
		static const Memory::PageSize MaxPageSize = static_cast<Memory::PageSize>(1ULL << MaxPageBits);

		static const Memory::PageMask MinPageMask = static_cast<Memory::PageMask>(MinPageSize - 1);
		static const Memory::PageMask MaxPageMask = static_cast<Memory::PageMask>(MaxPageSize - 1);
	}
}
#endif
