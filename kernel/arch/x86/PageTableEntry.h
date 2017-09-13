// PageTableEntry.h - Single entry in a page table.

#ifndef __ARCH_X86_PAGETABLEENTRY_H__
#define __ARCH_X86_PAGETABLEENTRY_H__

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

			constexpr static std::underlying_type<Flags>::type TypeFlags(Memory::MemType type)
			{
				switch(type)
				{
				case Memory::MemType::KERNEL_EXEC:
					return PAGE_PRESENT | PAGE_GLOBAL;
				case Memory::MemType::KERNEL_RO:
					return PAGE_PRESENT | PAGE_GLOBAL;
				case Memory::MemType::KERNEL_RW:
					return PAGE_PRESENT | PAGE_WRITEABLE | PAGE_GLOBAL;
				case Memory::MemType::KERNEL_PAGETAB:
					return PAGE_PRESENT | PAGE_WRITEABLE;
				case Memory::MemType::USER_EXEC:
					return PAGE_PRESENT | PAGE_USER;
				case Memory::MemType::USER_RO:
					return PAGE_PRESENT | PAGE_USER;
				case Memory::MemType::USER_RW:
					return PAGE_PRESENT | PAGE_WRITEABLE | PAGE_USER;
				case Memory::MemType::USER_PAGETAB:
					return PAGE_PRESENT | PAGE_WRITEABLE | PAGE_USER;
				case Memory::MemType::USER_COW:
					return PAGE_PRESENT | PAGE_USER;
				case Memory::MemType::USER_DEMAND:
					return PAGE_PRESENT | PAGE_USER;
				default:
					return 0;
				}
			}

		public:
			template<Memory::PageBits bits> PageTableEntry& Set(Memory::PhysAddr phys, Memory::MemType type)
			{
				static_assert(IsValidSize(bits), "invalid page size");
				if(bits == Memory::PGB_4K)
					raw = phys | TypeFlags(type);
				else
					raw = phys | TypeFlags(type) | PAGE_LARGE;
				return *this;
			}

#if defined ARCH_X86_AMD64 || defined CONFIG_PAE
			uint64_t Raw(void) const
#else
			uint32_t Raw(void) const
#endif
			{
				return raw;
			}

			Memory::PhysAddr Phys(void) const
			{
				return raw & (~0xfffULL);
			}

			void Clear(void)
			{
				raw = 0;
			}

			inline bool IsClear(void) const
			{
				return (raw == 0);
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

