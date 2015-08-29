// Chunker.h - Physical memory manager aka "chunker" function declarations.

#ifndef __CHUNKER_BITMAP_CHUNKER_H__
#define __CHUNKER_BITMAP_CHUNKER_H__

#include <Chunker.h>

namespace Kernel
{
	namespace Chunker
	{
		static const unsigned long ValidSizes = 1UL << Memory::MinPageBits;

		constexpr bool IsValidSize(Memory::PageBits size)
		{
			return Memory::IsValidSize(size, ValidSizes);
		}
	}
}

#endif
