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

#if defined ARCH_X86_AMD64 || defined CONFIG_PAE
			enum Flags : uint64_t
#else
			enum Flags : uint32_t
#endif
			{
				PAGE_PRESENT       = 0x0001,
				PAGE_WRITEABLE     = 0x0002,
				PAGE_USER          = 0x0004,
				PAGE_WRITE_THROUGH = 0x0008,
				PAGE_CACHE_DISABLE = 0x0010,
				PAGE_ACCESSED      = 0x0020,
				PAGE_DIRTY         = 0x0040,
				PAGE_LARGE         = 0x0080,
				PAGE_GLOBAL        = 0x0100
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
				return raw & (~0xfffULL);
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

			inline void SetPresent(bool x)
			{
				if(x)
					raw |= PAGE_PRESENT;
				else
					raw &= ~PAGE_PRESENT;
			}

			inline void SetWriteable(bool x)
			{
				if(x)
					raw |= PAGE_WRITEABLE;
				else
					raw &= ~PAGE_WRITEABLE;
			}

			inline void SetUser(bool x)
			{
				if(x)
					raw |= PAGE_USER;
				else
					raw &= ~PAGE_USER;
			}

			inline void SetAccessed(bool x)
			{
				if(x)
					raw |= PAGE_ACCESSED;
				else
					raw &= ~PAGE_ACCESSED;
			}

			inline void SetDirty(bool x)
			{
				if(x)
					raw |= PAGE_DIRTY;
				else
					raw &= ~PAGE_DIRTY;
			}

			inline void SetLarge(bool x)
			{
				if(x)
					raw |= PAGE_LARGE;
				else
					raw &= ~PAGE_LARGE;
			}

			inline void SetGlobal(bool x)
			{
				if(x)
					raw |= PAGE_GLOBAL;
				else
					raw &= ~PAGE_GLOBAL;
			}
		} PACKED;
	}
}

#endif
