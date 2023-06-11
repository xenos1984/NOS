// Chunker.h - Physical memory manager aka "chunker" function declarations.

#ifndef __CHUNKER_BITMAP_CHUNKER_H__
#define __CHUNKER_BITMAP_CHUNKER_H__

#include <type_traits>
#include <Memory.h>
#include <Pager.h>

namespace Kernel
{
	namespace Chunker
	{
		static const std::underlying_type<Memory::PageSize>::type ValidSizes = 1ULL << Pager::MinPageBits;
	}
}

#endif
