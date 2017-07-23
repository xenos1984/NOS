// Memory.h - General memory information and management.

#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <cstdint>

namespace Kernel
{
	namespace Memory
	{
		enum class MemType
		{
			KERNEL_EXEC,
			KERNEL_RO,
			KERNEL_RW,
			USER_EXEC,
			USER_RO,
			USER_RW,
			USER_COW,
			USER_DEMAND
		};

		enum PageBits : unsigned long
		{
			PGB_INV = 0,
			PGB_1K = 10,
			PGB_2K,
			PGB_4K,
			PGB_8K,
			PGB_16K,
			PGB_32K,
			PGB_64K,
			PGB_128K,
			PGB_256K,
			PGB_512K,
			PGB_1M,
			PGB_2M,
			PGB_4M,
			PGB_8M,
			PGB_16M,
			PGB_32M,
			PGB_64M,
			PGB_128M,
			PGB_256M,
			PGB_512M,
			PGB_1G,
			PGB_2G,
			PGB_4G,
			PGB_8G,
			PGB_16G,
			PGB_32G,
			PGB_64G,
			PGB_128G,
			PGB_256G,
			PGB_512G
		};

		enum PageSize : unsigned long long
		{
			PGS_1K = 1ULL << PGB_1K,
			PGS_2K = 1ULL << PGB_2K,
			PGS_4K = 1ULL << PGB_4K,
			PGS_8K = 1ULL << PGB_8K,
			PGS_16K = 1ULL << PGB_16K,
			PGS_32K = 1ULL << PGB_32K,
			PGS_64K = 1ULL << PGB_64K,
			PGS_128K = 1ULL << PGB_128K,
			PGS_256K = 1ULL << PGB_256K,
			PGS_512K = 1ULL << PGB_512K,
			PGS_1M = 1ULL << PGB_1M,
			PGS_2M = 1ULL << PGB_2M,
			PGS_4M = 1ULL << PGB_4M,
			PGS_8M = 1ULL << PGB_8M,
			PGS_16M = 1ULL << PGB_16M,
			PGS_32M = 1ULL << PGB_32M,
			PGS_64M = 1ULL << PGB_64M,
			PGS_128M = 1ULL << PGB_128M,
			PGS_256M = 1ULL << PGB_256M,
			PGS_512M = 1ULL << PGB_512M,
			PGS_1G = 1ULL << PGB_1G,
			PGS_2G = 1ULL << PGB_2G,
			PGS_4G = 1ULL << PGB_4G,
			PGS_8G = 1ULL << PGB_8G,
			PGS_16G = 1ULL << PGB_16G,
			PGS_32G = 1ULL << PGB_32G,
			PGS_64G = 1ULL << PGB_64G,
			PGS_128G = 1ULL << PGB_128G,
			PGS_256G = 1ULL << PGB_256G,
			PGS_512G = 1ULL << PGB_512G
		};

		enum PageMask : unsigned long long
		{
			PGM_1K = PGS_1K - 1,
			PGM_2K = PGS_2K - 1,
			PGM_4K = PGS_4K - 1,
			PGM_8K = PGS_8K - 1,
			PGM_16K = PGS_16K - 1,
			PGM_32K = PGS_32K - 1,
			PGM_64K = PGS_64K - 1,
			PGM_128K = PGS_128K - 1,
			PGM_256K = PGS_256K - 1,
			PGM_512K = PGS_512K - 1,
			PGM_1M = PGS_1M - 1,
			PGM_2M = PGS_2M - 1,
			PGM_4M = PGS_4M - 1,
			PGM_8M = PGS_8M - 1,
			PGM_16M = PGS_16M - 1,
			PGM_32M = PGS_32M - 1,
			PGM_64M = PGS_64M - 1,
			PGM_128M = PGS_128M - 1,
			PGM_256M = PGS_256M - 1,
			PGM_512M = PGS_512M - 1,
			PGM_1G = PGS_1G - 1,
			PGM_2G = PGS_2G - 1,
			PGM_4G = PGS_4G - 1,
			PGM_8G = PGS_8G - 1,
			PGM_16G = PGS_16G - 1,
			PGM_32G = PGS_32G - 1,
			PGM_64G = PGS_64G - 1,
			PGM_128G = PGS_128G - 1,
			PGM_256G = PGS_256G - 1,
			PGM_512G = PGS_512G - 1
		};

		constexpr bool IsValidSize(PageBits size, unsigned long mask)
		{
			return((mask & (1UL << size)) != 0);
		}
	}
}

#include INC_ARCH(Memory.h)

namespace Kernel
{
	namespace Memory
	{
		template<PageBits bits> void* AllocBlock(uintptr_t virt, MemType type, Memory::Zone zone = static_cast<Memory::Zone>(0));
		template<PageBits bits> inline void* AllocBlock(void* virt, MemType type, Memory::Zone zone = static_cast<Memory::Zone>(0)) { return AllocBlock<bits>((uintptr_t)virt, type, zone); }
		template<PageBits bits> bool FreeBlock(uintptr_t virt);
		template<PageBits bits> inline bool FreeBlock(void* virt) { return FreeBlock<bits>((uintptr_t)virt); }
	}
}

#endif
