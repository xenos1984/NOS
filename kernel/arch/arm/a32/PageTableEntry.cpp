// PageTableEntry.cpp - Single entry in a page table.

#include INC_BITS(PageTableEntry.h)
#include <Memory.h>

namespace Kernel
{
	namespace Pager
	{
		template std::underlying_type<PageTableEntryL1::Flags>::type PageTableEntryL1::TypeFlags<Memory::PGB_16M>(Memory::MemType type);
		template std::underlying_type<PageTableEntryL1::Flags>::type PageTableEntryL1::TypeFlags<Memory::PGB_1M>(Memory::MemType type);
		template std::underlying_type<PageTableEntryL2::Flags>::type PageTableEntryL2::TypeFlags<Memory::PGB_64K>(Memory::MemType type);
		template std::underlying_type<PageTableEntryL2::Flags>::type PageTableEntryL2::TypeFlags<Memory::PGB_4K>(Memory::MemType type);
	}
}
