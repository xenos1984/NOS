// Segment.h - Memory segment

#ifndef __SEGMENT_H__
#define __SEGMENT_H__

#include <cstdint>
#include <Memory.h>

namespace Kernel
{
	namespace Memory
	{
		class Segment
		{
		public:
			uintptr_t start;
			uintptr_t length;
			Segment* prev;
			Segment* next;
			MemType type;
		};
	}
}

#endif
