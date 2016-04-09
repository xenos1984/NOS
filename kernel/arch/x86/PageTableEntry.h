// PageTableEntry.h - Single entry in a page table.

#ifndef __ARCH_X86_PAGETABLEENTRY_H__
#define __ARCH_X86_PAGETABLEENTRY_H__

#include <Memory.h>

namespace Kernel
{
	namespace Pager
	{
		class PageTableEntry
		{
		private:
#if defined ARCH_X86_AMD64 || defined CONFIG_PAE
			uint64_t raw = 0;
#else
			uint32_t raw = 0;
#endif

			enum Flags : unsigned long
			{
				PAGE_PRESENT       = 0x0001UL,
				PAGE_WRITEABLE     = 0x0002UL,
				PAGE_USER          = 0x0004UL,
				PAGE_WRITE_THROUGH = 0x0008UL,
				PAGE_CACHE_DISABLE = 0x0010UL,
				PAGE_ACCESSED      = 0x0020UL,
				PAGE_DIRTY         = 0x0040UL,
				PAGE_LARGE         = 0x0080UL,
				PAGE_GLOBAL        = 0x0100UL
			};

		public:
/*			static PageTableEntry& Entry(int i)
			{
				return reinterpret_cast<PageTableEntry*>(REC_PAGE_TAB)[i];
			}
*/
			PageTableEntry& operator=(Memory::PhysAddr phys)
			{
				raw = phys | PAGE_PRESENT;
				return *this;
			}

			Memory::PhysAddr Phys(void) const
			{
				return raw & (~4095ULL);
			}

			void Clear(void)
			{
				raw = 0;
			}

			inline bool IsPresent(void) const
			{
				return (bool)(raw & PAGE_PRESENT);
			}

			inline bool IsWriteable(void) const
			{
				return (bool)(raw & PAGE_WRITEABLE);
			}

			inline bool IsUser(void) const
			{
				return (bool)(raw & PAGE_USER);
			}

			inline bool IsAccessed(void) const
			{
				return (bool)(raw & PAGE_ACCESSED);
			}

			inline bool IsDirty(void) const
			{
				return (bool)(raw & PAGE_DIRTY);
			}

			inline bool IsLarge(void) const
			{
				return (bool)(raw & PAGE_LARGE);
			}

			inline bool IsGlobal(void) const
			{
				return (bool)(raw & PAGE_GLOBAL);
			}
		} PACKED;
	}
}

#endif

