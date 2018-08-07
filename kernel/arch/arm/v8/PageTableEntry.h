// PageTableEntry.h - Single entry in a page table.

#ifndef __ARCH_ARM_V8_PAGETABLEENTRY_H__
#define __ARCH_ARM_V8_PAGETABLEENTRY_H__

#include <type_traits>
#include <Memory.h>
#include <Pager.h>

namespace Kernel
{
	namespace Pager
	{
		class PageTableEntry
		{
		private:
			uint64_t raw = 0;

			enum Flags : uint64_t
			{
				PAGE_INVALID    = 0x0000000000000000,
				PAGE_BLOCK      = 0x0000000000000001,
				PAGE_TABLE      = 0x0000000000000003,
				PAGE_PAGE       = 0x0000000000000003,
				PAGE_ATTR_MASK  = 0x000000000000001c,
				PAGE_NSECURE    = 0x0000000000000020,
				PAGE_KERNEL     = 0x0000000000000000,
				PAGE_USER       = 0x0000000000000040,
				PAGE_RW         = 0x0000000000000000,
				PAGE_RO         = 0x0000000000000080,
				PAGE_SH_NONE    = 0x0000000000000000,
				PAGE_SH_OUTER   = 0x0000000000000200,
				PAGE_SH_INNER   = 0x0000000000000300,
				PAGE_ACCESSED   = 0x0000000000000400,
				PAGE_NGLOBAL    = 0x0000000000000800,
				PAGE_DIRTY      = 0x0008000000000000,
				PAGE_CONTIGUOUS = 0x0010000000000000,
				PAGE_K_NOEXEC   = 0x0020000000000000,
				PAGE_U_NOEXEC   = 0x0040000000000000
			};

			template<Memory::PageBits bits> constexpr static std::underlying_type<Flags>::type TypeFlags(Memory::MemType type);

		public:
			template<Memory::PageBits bits> inline PageTableEntry& Set(Memory::PhysAddr phys, Memory::MemType type);

			inline uint64_t Raw(void) const
			{
				return raw;
			}

			inline void Clear(void)
			{
				raw = 0;
			}

			inline bool IsClear(void) const
			{
				return (raw == 0);
			}
		};
	}
}

#endif
