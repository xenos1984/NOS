// PageTableEntry.h - Single entry in a page table.

#ifndef __ARCH_ARM_V7_PAGETABLEENTRY_H__
#define __ARCH_ARM_V7_PAGETABLEENTRY_H__

#include <type_traits>
#include <Memory.h>
#include <Pager.h>

namespace Kernel
{
	namespace Pager
	{
		class PageTableEntryL1
		{
		private:
			uint32_t raw = 0;

			enum Flags : uint32_t
			{
				PAGE_INVALID       = 0x00000000,
				PAGE_TABLE         = 0x00000001,
				PAGE_SECTION       = 0x00000002,
				PAGE_SUPER         = 0x00040002,
				PAGE_T_PNOEXEC     = 0x00000004,
				PAGE_S_PNOEXEC     = 0x00000001,
				PAGE_S_BUFFER      = 0x00000004,
				PAGE_S_CACHE       = 0x00000008,
				PAGE_S_NOEXEC      = 0x00000010,
				PAGE_T_NONSEC      = 0x00000008,
				PAGE_S_NONSEC      = 0x00080000,
				PAGE_S_SHARE       = 0x00010000,
				PAGE_S_NGLOBAL     = 0x00020000,
				PAGE_ACC_K_NA_U_NA = 0x00000000,
				PAGE_ACC_K_RW_U_NA = 0x00000400,
				PAGE_ACC_K_RW_U_RO = 0x00000800,
				PAGE_ACC_K_RW_U_RW = 0x00000c00,
				PAGE_ACC_K_RO_U_NA = 0x00008400,
				PAGE_ACC_K_RO_U_RO = 0x00008c00,
				PAGE_MRA_STRORD    = 0x00000000,
				PAGE_MRA_DEV_SHA   = 0x00000004,
				PAGE_MRA_DEV_NSHA  = 0x00002000,
				PAGE_MRA_IMPL      = 0x00001008,
				PAGE_MRA_N_NC      = 0x00001000,
				PAGE_MRA_N_WB_WA   = 0x0000100c,
				PAGE_MRA_N_WB_NWA  = 0x0000000c,
				PAGE_MRA_N_WT_NWA  = 0x00000008,
				PAGE_MRA_N_CACHE   = 0x00004000,
				PAGE_MRA_I_NC      = 0x00000000,
				PAGE_MRA_I_WB_WA   = 0x00000004,
				PAGE_MRA_I_WB_NWA  = 0x0000000c,
				PAGE_MRA_I_WT_NWA  = 0x00000008,
				PAGE_MRA_O_NC      = 0x00000000,
				PAGE_MRA_O_WB_WA   = 0x00001000,
				PAGE_MRA_O_WB_NWA  = 0x00003000,
				PAGE_MRA_O_WT_NWA  = 0x00002000
			};

			template<Memory::PageBits bits> constexpr static std::underlying_type<Flags>::type TypeFlags(Memory::MemType type);

		public:
			template<Memory::PageBits bits> PageTableEntryL1& Set(Memory::PhysAddr phys, Memory::MemType type);

			inline uint32_t Raw(void) const
			{
				return raw;
			}

			inline Memory::PhysAddr Phys(void) const
			{
				return raw & (~0xfffULL);
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

		class PageTableEntryL2
		{
		private:
			uint32_t raw = 0;

			enum Flags : uint32_t
			{
				PAGE_INVALID        = 0x0000,
				PAGE_LARGE          = 0x0001,
				PAGE_SMALL          = 0x0002,
				PAGE_BUFFER         = 0x0004,
				PAGE_CACHE          = 0x0008,
				PAGE_SHARE          = 0x0400,
				PAGE_NGLOBAL        = 0x0800,
				PAGE_S_NOEXEC       = 0x0001,
				PAGE_L_NOEXEC       = 0x8000,
				PAGE_ACC_K_NA_U_NA  = 0x0000,
				PAGE_ACC_K_RW_U_NA  = 0x0010,
				PAGE_ACC_K_RW_U_RO  = 0x0020,
				PAGE_ACC_K_RW_U_RW  = 0x0030,
				PAGE_ACC_K_RO_U_NA  = 0x0210,
				PAGE_ACC_K_RO_U_RO  = 0x0230,
				PAGE_L_MRA_STRORD   = 0x00000000,
				PAGE_L_MRA_DEV_SHA  = 0x00000004,
				PAGE_L_MRA_DEV_NSHA = 0x00002000,
				PAGE_L_MRA_IMPL     = 0x00001008,
				PAGE_L_MRA_N_NC     = 0x00001000,
				PAGE_L_MRA_N_WB_WA  = 0x0000100c,
				PAGE_L_MRA_N_WB_NWA = 0x0000000c,
				PAGE_L_MRA_N_WT_NWA = 0x00000008,
				PAGE_L_MRA_N_CACHE  = 0x00004000,
				PAGE_L_MRA_I_NC     = 0x00000000,
				PAGE_L_MRA_I_WB_WA  = 0x00000004,
				PAGE_L_MRA_I_WB_NWA = 0x0000000c,
				PAGE_L_MRA_I_WT_NWA = 0x00000008,
				PAGE_L_MRA_O_NC     = 0x00000000,
				PAGE_L_MRA_O_WB_WA  = 0x00001000,
				PAGE_L_MRA_O_WB_NWA = 0x00003000,
				PAGE_L_MRA_O_WT_NWA = 0x00002000,
				PAGE_S_MRA_STRORD   = 0x00000000,
				PAGE_S_MRA_DEV_SHA  = 0x00000004,
				PAGE_S_MRA_DEV_NSHA = 0x00000080,
				PAGE_S_MRA_IMPL     = 0x00000048,
				PAGE_S_MRA_N_NC     = 0x00000040,
				PAGE_S_MRA_N_WB_WA  = 0x0000004c,
				PAGE_S_MRA_N_WB_NWA = 0x0000000c,
				PAGE_S_MRA_N_WT_NWA = 0x00000008,
				PAGE_S_MRA_N_CACHE  = 0x00000100,
				PAGE_S_MRA_I_NC     = 0x00000000,
				PAGE_S_MRA_I_WB_WA  = 0x00000004,
				PAGE_S_MRA_I_WB_NWA = 0x0000000c,
				PAGE_S_MRA_I_WT_NWA = 0x00000008,
				PAGE_S_MRA_O_NC     = 0x00000000,
				PAGE_S_MRA_O_WB_WA  = 0x00000040,
				PAGE_S_MRA_O_WB_NWA = 0x000000c0,
				PAGE_S_MRA_O_WT_NWA = 0x00000080
			};

			template<Memory::PageBits bits> constexpr static std::underlying_type<Flags>::type TypeFlags(Memory::MemType type);

		public:
			template<Memory::PageBits bits> PageTableEntryL2& Set(Memory::PhysAddr phys, Memory::MemType type);

			inline uint32_t Raw(void) const
			{
				return raw;
			}

			inline Memory::PhysAddr Phys(void) const
			{
				return raw & (~0xfffULL);
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

		template<Memory::PageBits bits> constexpr std::underlying_type<PageTableEntryL1::Flags>::type PageTableEntryL1::TypeFlags(Memory::MemType type)
		{
			static_assert((bits == Memory::PGB_1M) || (bits == Memory::PGB_16M));
			return 0;
		}

		template<> constexpr std::underlying_type<PageTableEntryL1::Flags>::type PageTableEntryL1::TypeFlags<Memory::PGB_1M>(Memory::MemType type)
		{
			switch(type)
			{
			case Memory::MemType::KERNEL_EXEC:
				return PAGE_SECTION | PAGE_ACC_K_RO_U_NA | PAGE_MRA_N_WB_WA | PAGE_S_SHARE;
			case Memory::MemType::KERNEL_RO:
				return PAGE_SECTION | PAGE_ACC_K_RO_U_NA | PAGE_MRA_N_WB_WA | PAGE_S_NOEXEC | PAGE_S_SHARE;
			case Memory::MemType::KERNEL_RW:
				return PAGE_SECTION | PAGE_ACC_K_RW_U_NA | PAGE_MRA_N_WB_WA | PAGE_S_NOEXEC | PAGE_S_SHARE;
			case Memory::MemType::KERNEL_PAGETAB:
				return PAGE_INVALID;
			case Memory::MemType::USER_EXEC:
				return PAGE_SECTION | PAGE_ACC_K_RW_U_RO | PAGE_MRA_N_WB_WA | PAGE_S_SHARE;
			case Memory::MemType::USER_RO:
				return PAGE_SECTION | PAGE_ACC_K_RW_U_RO | PAGE_MRA_N_WB_WA | PAGE_S_NOEXEC | PAGE_S_SHARE;
			case Memory::MemType::USER_RW:
				return PAGE_SECTION | PAGE_ACC_K_RW_U_RW | PAGE_MRA_N_WB_WA | PAGE_S_NOEXEC | PAGE_S_SHARE;
			case Memory::MemType::USER_PAGETAB:
				return PAGE_INVALID;
			case Memory::MemType::USER_COW:
				return PAGE_SECTION | PAGE_ACC_K_RW_U_RO | PAGE_MRA_N_WB_WA | PAGE_S_NOEXEC | PAGE_S_SHARE;
			case Memory::MemType::USER_DEMAND:
				return PAGE_SECTION | PAGE_ACC_K_RW_U_RO | PAGE_MRA_N_WB_WA | PAGE_S_NOEXEC | PAGE_S_SHARE;
			default:
				return 0;
			}
		}

		template<> constexpr std::underlying_type<PageTableEntryL1::Flags>::type PageTableEntryL1::TypeFlags<Memory::PGB_16M>(Memory::MemType type)
		{
			switch(type)
			{
			case Memory::MemType::KERNEL_EXEC:
				return PAGE_SUPER | PAGE_ACC_K_RO_U_NA | PAGE_MRA_N_WB_WA | PAGE_S_SHARE;
			case Memory::MemType::KERNEL_RO:
				return PAGE_SUPER | PAGE_ACC_K_RO_U_NA | PAGE_MRA_N_WB_WA | PAGE_S_NOEXEC | PAGE_S_SHARE;
			case Memory::MemType::KERNEL_RW:
				return PAGE_SUPER | PAGE_ACC_K_RW_U_NA | PAGE_MRA_N_WB_WA | PAGE_S_NOEXEC | PAGE_S_SHARE;
			case Memory::MemType::KERNEL_PAGETAB:
				return PAGE_INVALID;
			case Memory::MemType::USER_EXEC:
				return PAGE_SUPER | PAGE_ACC_K_RW_U_RO | PAGE_MRA_N_WB_WA | PAGE_S_SHARE;
			case Memory::MemType::USER_RO:
				return PAGE_SUPER | PAGE_ACC_K_RW_U_RO | PAGE_MRA_N_WB_WA | PAGE_S_NOEXEC | PAGE_S_SHARE;
			case Memory::MemType::USER_RW:
				return PAGE_SUPER | PAGE_ACC_K_RW_U_RW | PAGE_MRA_N_WB_WA | PAGE_S_NOEXEC | PAGE_S_SHARE;
			case Memory::MemType::USER_PAGETAB:
				return PAGE_INVALID;
			case Memory::MemType::USER_COW:
				return PAGE_SUPER | PAGE_ACC_K_RW_U_RO | PAGE_MRA_N_WB_WA | PAGE_S_NOEXEC | PAGE_S_SHARE;
			case Memory::MemType::USER_DEMAND:
				return PAGE_SUPER | PAGE_ACC_K_RW_U_RO | PAGE_MRA_N_WB_WA | PAGE_S_NOEXEC | PAGE_S_SHARE;
			default:
				return 0;
			}
		}

		template<Memory::PageBits bits> PageTableEntryL1& PageTableEntryL1::Set(Memory::PhysAddr phys, Memory::MemType type)
		{
			static_assert((bits == Memory::PGB_1M) || (bits == Memory::PGB_16M), "invalid page size for L2 table");
			raw = phys | TypeFlags<bits>(type);
			return *this;
		}

		template<Memory::PageBits bits> constexpr std::underlying_type<PageTableEntryL2::Flags>::type PageTableEntryL2::TypeFlags(Memory::MemType type)
		{
			static_assert((bits == Memory::PGB_4K) || (bits == Memory::PGB_64K));
			return 0;
		}

		template<> constexpr std::underlying_type<PageTableEntryL2::Flags>::type PageTableEntryL2::TypeFlags<Memory::PGB_4K>(Memory::MemType type)
		{
			switch(type)
			{
			case Memory::MemType::KERNEL_EXEC:
				return PAGE_SMALL | PAGE_ACC_K_RO_U_NA | PAGE_S_MRA_N_WB_WA | PAGE_SHARE;
			case Memory::MemType::KERNEL_RO:
				return PAGE_SMALL | PAGE_ACC_K_RO_U_NA | PAGE_S_MRA_N_WB_WA | PAGE_S_NOEXEC | PAGE_SHARE;
			case Memory::MemType::KERNEL_RW:
				return PAGE_SMALL | PAGE_ACC_K_RW_U_NA | PAGE_S_MRA_N_WB_WA | PAGE_S_NOEXEC | PAGE_SHARE;
			case Memory::MemType::KERNEL_PAGETAB:
				return PAGE_INVALID;
			case Memory::MemType::USER_EXEC:
				return PAGE_SMALL | PAGE_ACC_K_RW_U_RO | PAGE_S_MRA_N_WB_WA | PAGE_SHARE;
			case Memory::MemType::USER_RO:
				return PAGE_SMALL | PAGE_ACC_K_RW_U_RO | PAGE_S_MRA_N_WB_WA | PAGE_S_NOEXEC | PAGE_SHARE;
			case Memory::MemType::USER_RW:
				return PAGE_SMALL | PAGE_ACC_K_RW_U_RW | PAGE_S_MRA_N_WB_WA | PAGE_S_NOEXEC | PAGE_SHARE;
			case Memory::MemType::USER_PAGETAB:
				return PAGE_INVALID;
			case Memory::MemType::USER_COW:
				return PAGE_SMALL | PAGE_ACC_K_RW_U_RO | PAGE_S_MRA_N_WB_WA | PAGE_S_NOEXEC | PAGE_SHARE;
			case Memory::MemType::USER_DEMAND:
				return PAGE_SMALL | PAGE_ACC_K_RW_U_RO | PAGE_S_MRA_N_WB_WA | PAGE_S_NOEXEC | PAGE_SHARE;
			default:
				return 0;
			}
		}

		template<> constexpr std::underlying_type<PageTableEntryL2::Flags>::type PageTableEntryL2::TypeFlags<Memory::PGB_64K>(Memory::MemType type)
		{
			switch(type)
			{
			case Memory::MemType::KERNEL_EXEC:
				return PAGE_LARGE | PAGE_ACC_K_RO_U_NA | PAGE_L_MRA_N_WB_WA | PAGE_SHARE;
			case Memory::MemType::KERNEL_RO:
				return PAGE_LARGE | PAGE_ACC_K_RO_U_NA | PAGE_L_MRA_N_WB_WA | PAGE_L_NOEXEC | PAGE_SHARE;
			case Memory::MemType::KERNEL_RW:
				return PAGE_LARGE | PAGE_ACC_K_RW_U_NA | PAGE_L_MRA_N_WB_WA | PAGE_L_NOEXEC | PAGE_SHARE;
			case Memory::MemType::KERNEL_PAGETAB:
				return PAGE_INVALID;
			case Memory::MemType::USER_EXEC:
				return PAGE_LARGE | PAGE_ACC_K_RW_U_RO | PAGE_L_MRA_N_WB_WA | PAGE_SHARE;
			case Memory::MemType::USER_RO:
				return PAGE_LARGE | PAGE_ACC_K_RW_U_RO | PAGE_L_MRA_N_WB_WA | PAGE_L_NOEXEC | PAGE_SHARE;
			case Memory::MemType::USER_RW:
				return PAGE_LARGE | PAGE_ACC_K_RW_U_RW | PAGE_L_MRA_N_WB_WA | PAGE_L_NOEXEC | PAGE_SHARE;
			case Memory::MemType::USER_PAGETAB:
				return PAGE_INVALID;
			case Memory::MemType::USER_COW:
				return PAGE_LARGE | PAGE_ACC_K_RW_U_RO | PAGE_L_MRA_N_WB_WA | PAGE_L_NOEXEC | PAGE_SHARE;
			case Memory::MemType::USER_DEMAND:
				return PAGE_LARGE | PAGE_ACC_K_RW_U_RO | PAGE_L_MRA_N_WB_WA | PAGE_L_NOEXEC | PAGE_SHARE;
			default:
				return 0;
			}
		}

		template<Memory::PageBits bits> PageTableEntryL2& PageTableEntryL2::Set(Memory::PhysAddr phys, Memory::MemType type)
		{
			static_assert((bits == Memory::PGB_4K) || (bits == Memory::PGB_64K), "invalid page size for L2 table");
			raw = phys | TypeFlags<bits>(type);
			return *this;
		}
	}
}

#endif

