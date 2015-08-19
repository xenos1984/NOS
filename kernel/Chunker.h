// Chunker.h - Physical memory manager aka "chunker" function declarations.

#ifndef __CHUNKER_H__
#define __CHUNKER_H__

#include <Memory.h>

namespace Kernel
{
	namespace Chunker
	{
		void Init(Memory::PhysAddr start, Memory::PhysAddr length, Memory::Zone zone);
		void AddRegion(Memory::PhysAddr start, Memory::PhysAddr length, Memory::Zone zone);

		Memory::PhysAddr Alloc(Memory::Zone zone = static_cast<Memory::Zone>(0));
		bool Free(Memory::PhysAddr addr);
		bool Free(Memory::PhysAddr start, Memory::PhysAddr length);
		bool Reserve(Memory::PhysAddr addr);
		bool Reserve(Memory::PhysAddr start, Memory::PhysAddr length);
	}
}

#endif
