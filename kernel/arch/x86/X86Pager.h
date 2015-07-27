// X86Pager.h - Physical memory management for x86 architectures.

#ifndef __ARCH_X86_X86PAGER_H__
#define __ARCH_X86_X86PAGER_H__

#include INC_ARCH(Multiboot.h)
#include <PhysicalMemory.h>

extern "C" unsigned long tabPGTAB[];
extern "C" unsigned long tabPGDIR[];
#ifdef ARCH_X86_AMD64
extern "C" unsigned long tabPGDIP[];
extern "C" unsigned long tabPML4T[];
#endif
extern "C" unsigned long tabBITS[];

namespace Kernel
{
	class X86Pager : public PhysicalMemory
	{
	public:
		static const unsigned int  PAGE_SIZE       = 4096; /**< Size of a physical page. */
		static const unsigned long PAGE_SIZE_MASK  = 4095; /**< Mask to cover all lower bits (page flags). */
		static const unsigned int  PAGE_SIZE_SHIFT = 12; /**< Shift required to get from page number to address. */
#ifdef ARCH_X86_IA32
		static const unsigned int  PAGE_BITS       = 10; /**< Number of bits covered by each page table level. */
		static const unsigned int  TABLE_LEVELS    = 2; /**< Number of levels in the page table hierarchy. */
		static const unsigned int  PAGE_RECURSIVE  = 1023; /**< Slot in the page table used for recursive page tables. */
#endif
#ifdef ARCH_X86_AMD64
		static const unsigned int  PAGE_BITS       = 9; /**< Number of bits covered by each page table level. */
		static const unsigned int  TABLE_LEVELS    = 4; /**< Number of levels in the page table hierarchy. */
		static const unsigned int  PAGE_RECURSIVE  = 510; /**< Slot in the page table used for recursive page tables. */
#endif
		static const unsigned int  TABLE_BITS      = PAGE_BITS * TABLE_LEVELS; /**< Total number of bits referencing a page. */
		static const unsigned int  PAGES_PER_TABLE = 1UL << PAGE_BITS; /**< Number of pages in each page table. */
		static const unsigned long PAGE_COUNT      = 1UL << TABLE_BITS; /**< Total number of pages in virtual address space. */
		static const unsigned long TABLE_COUNT     = PAGE_COUNT >> PAGE_BITS; /**< Total number of page tables in virtual address space. */
#ifdef ARCH_X86_IA32
		static const uintptr_t pageTab = (uintptr_t)PAGE_RECURSIVE << (PAGE_SIZE_SHIFT + PAGE_BITS); /**< Recursive base address of page tables. */
		static const uintptr_t pageDir = pageTab + ((uintptr_t)PAGE_RECURSIVE << PAGE_SIZE_SHIFT); /**< Recursive base address of page directory. */
		static const uintptr_t pageTop = pageDir; /**< Recursive base address of top level page table. */
#endif
#ifdef ARCH_X86_AMD64
		static const uintptr_t pageTab = 0xffff000000000000UL + ((uintptr_t)PAGE_RECURSIVE << (PAGE_SIZE_SHIFT + 3 * PAGE_BITS)); /**< Recursive base address of page tables. */
		static const uintptr_t pageDir = pageTab + ((uintptr_t)PAGE_RECURSIVE << (PAGE_SIZE_SHIFT + 2 * PAGE_BITS)); /**< Recursive base address of page directories. */
		static const uintptr_t pageDip = pageDir + ((uintptr_t)PAGE_RECURSIVE << (PAGE_SIZE_SHIFT + PAGE_BITS)); /**< Recursive base address of page directory pointers. */
		static const uintptr_t pageL4t = pageDip + ((uintptr_t)PAGE_RECURSIVE << PAGE_SIZE_SHIFT); /**< Recursive base address of level 4 table. */
		static const uintptr_t pageTop = pageL4t; /**< Recursive base address of top level page table. */
#endif
		static const uintptr_t PAGE_TOP_LEVEL  = (pageTop - pageTab) / sizeof(long);
		static const uintptr_t PAGE_TOP_REC    = PAGE_TOP_LEVEL + PAGE_RECURSIVE;

		struct MemoryArea
		{
			MemoryArea* prev;
			MemoryArea* next;
			uintptr_t start;
			uintptr_t length;
			uintptr_t offset;
			unsigned long* bitmap;

			MemoryArea(uintptr_t s, uintptr_t l, unsigned long* b);

			void Mark(unsigned long first, unsigned long last, bool alloc);
			void Mark(unsigned long page, bool alloc);
			void Mark(bool alloc);
			bool IsAlloc(unsigned long page);
			unsigned long AllocBlocks(unsigned long count);
			unsigned long AllocBlocks(unsigned long count, unsigned long first, unsigned long last);
		};

		union PageTableEntry
		{
			struct
			{
				unsigned long present : 1;
				unsigned long writeable : 1;
				unsigned long user : 1;
				unsigned long writethrough : 1;
				unsigned long cachedisable : 1;
				unsigned long accessed : 1;
				unsigned long dirty : 1;
				unsigned long large : 1;
				unsigned long global : 1;
				unsigned long reserved : 3;
				unsigned long page: 8 * sizeof(long) - 12;
			} bits PACKED;
			unsigned long raw;
		};

		struct PageTable
		{
			PageTableEntry page[PAGES_PER_TABLE];
		} PACKED;

	private:
		MemoryArea basemem;
		uintptr_t allocendvirt, allocendphys;
		unsigned long membelow4g, pagesbelow4g, memtotal, pagestotal;

		inline PageTableEntry& TopPages(unsigned long n)
		{
			return(((PageTableEntry*)pageTop)[n]);
		};

		inline PageTableEntry& Pages(unsigned long n)
		{
			return(((PageTableEntry*)pageTab)[n]);
		};

		inline PageTable& PageTables(unsigned long n)
		{
			return(((PageTable*)pageTab)[n]);
		};

		/** Checks whether this page holds a page table. */
		inline bool IsTablePage(unsigned long n)
		{
			return(n >> (PAGE_BITS * (TABLE_LEVELS - 1)) == PAGE_RECURSIVE);
		}

		/** Checks whether this page is listed in the top level page table. */
		inline bool IsTopPage(unsigned long n)
		{
			return(n >> PAGE_BITS == PAGE_TOP_LEVEL >> PAGE_BITS);
		}

		inline void InvalidatePage(unsigned long addr)
		{
			asm volatile ("invlpg (%0)" : : "r"(addr));
		}

		bool PageExists(unsigned long n);
		bool TableExists(unsigned long n);
		bool TableEmpty(unsigned long n);
		void CreateTable(unsigned long n);
		void DestroyTable(unsigned long n);

	public:
		X86Pager(Multiboot::Info* mbi);

		unsigned long GetBlockSize(void);

		void* AllocBlocks(void* virt, unsigned long n, unsigned long first, unsigned long last, bool cont);
		void* AllocBlocks(void* virt, unsigned long n);
		void FreeBlocks(void* virt, unsigned long n);
		void Mark(unsigned long phys, bool alloc);

		bool VirtExists(void* addr);
		unsigned long VirtToPhys(void* addr);
		void* MapToLinear(void* phys, void* virt, unsigned long n);
		void Unmap(void* virt, unsigned long n);

		void* MapTempRegion(unsigned long start, unsigned long length);
		void* MapTempString(unsigned long start);
		void AddMemoryArea(Multiboot::MemoryMap* mem);
		void* PhysToVirt(unsigned long addr);

		PageTable* CreateContext(void);
		unsigned long SwitchContext(unsigned long phys);
		void DeleteContext(PageTable* pg);

		void CleanInit(void);
	};
}

inline Kernel::X86Pager& x86pager(void) { return(reinterpret_cast<Kernel::X86Pager&>(physmem_space)); }

#endif
