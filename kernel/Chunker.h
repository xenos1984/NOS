// Chunker.h - Physical memory manager aka "chunker" function declarations.

#ifndef __CHUNKER_H__
#define __CHUNKER_H__

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

#endif
