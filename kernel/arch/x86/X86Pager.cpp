// X86Pager.cpp - Physical memory management for x86 architectures.

#include <cstddef>
#include INC_ARCH(X86Pager.h)
#include <Process.h>
#include INC_ARCH(Multiboot.h)
#include INC_ARCH(ControlRegisters.h)
#include <Symbol.h>
#include <Heap.h>
#include <Console.h>

using namespace Kernel;

char physmem_space[sizeof(X86Pager)];

X86Pager::MemoryArea::MemoryArea(uintptr_t s, uintptr_t l, unsigned long* b)
{
	start = s;
	length = l;
	bitmap = b;
	prev = next = this;
}

bool X86Pager::MemoryArea::IsAlloc(unsigned long page)
{
	if(bitmap == nullptr)
		return true;

	return((bitmap[page / sizeof(long) / 8] & (1UL << (page & (8 * sizeof(long) - 1)))) != 0);
}

void X86Pager::MemoryArea::Mark(unsigned long page, bool alloc)
{
	if(alloc)
		bitmap[page / sizeof(long) / 8] |= (1UL << (page & (8 * sizeof(long) - 1)));
	else
		bitmap[page / sizeof(long) / 8] &= ~(1UL << (page & (8 * sizeof(long) - 1)));
}

void X86Pager::MemoryArea::Mark(unsigned long first, unsigned long last, bool alloc)
{
	unsigned long p, p1, p2;

	if(first >= start + length)
		return;

	if(last <= start)
		return;

//	Console::WriteFormat("Mark(0x%16lx, 0x%16lx, %d)\n", first, last, alloc);

	if(first < start)
		p1 = 0;
	else
		p1 = (first - start) >> PAGE_SIZE_SHIFT;

	if(last > start + length)
		p2 = (length - 1) >> PAGE_SIZE_SHIFT;
	else
		p2 = ((last - start) - 1) >> PAGE_SIZE_SHIFT;

	for(p = p1; p <= p2; p++)
		Mark(p, alloc);
}

void X86Pager::MemoryArea::Mark(bool alloc)
{
	unsigned long i;

	for(i = 0; i < length / (PAGE_SIZE * sizeof(long) * 8); i++)
		bitmap[i] = (alloc ? ~0UL : 0UL);
}

unsigned long X86Pager::MemoryArea::AllocBlocks(unsigned long count)
{
	return(AllocBlocks(count, start, start + length));
}

unsigned long X86Pager::MemoryArea::AllocBlocks(unsigned long count, unsigned long first, unsigned long last)
{
	unsigned long i, k, p1, p2;
	bool ok;

	if(bitmap == nullptr)
		return(0);

	if(last <= start)
		return(0);

	if(first >= start + length)
		return(0);

	if(first < start)
		p1 = 0;
	else
		p1 = (((first - start - 1) >> PAGE_SIZE_SHIFT) + 1) & (PAGE_COUNT - 1);

	if(last > start + length)
		p2 = (length >> PAGE_SIZE_SHIFT) - 1;
	else
		p2 = ((last - start) >> PAGE_SIZE_SHIFT) - 1;

	for(i = p2; (i >= p1 + count - 1) && (i <= p2); i--)
	{
		if(!IsAlloc(i))
		{
			ok = true;
			for(k = 1; k < count; k++)
			{
				if(IsAlloc(i - k))
				{
					ok = false;
					break;
				}
			}
			if(ok)
			{
				for(k = 0; k < count; k++)
					Mark(i - k, true);
				return(start + PAGE_SIZE * (i - count + 1));
			}
		}
	}
	return(0);
}

SECTION(".init.text") X86Pager::X86Pager(Multiboot::Info* mbi) : basemem(0, 0, nullptr)
{
	Multiboot::Module* mmod;
	Multiboot::Info* info = (Multiboot::Info*)((unsigned long)mbi + Symbol::kernelOffset.Addr());
/*
	Console::WriteFormat("pageTab = 0x%16lx\n", pageTab);
	Console::WriteFormat("pageDir = 0x%16lx\n", pageDir);
	Console::WriteFormat("pageDip = 0x%16lx\n", pageDip);
	Console::WriteFormat("pageL4t = 0x%16lx\n", pageL4t);
	Console::WriteFormat("pageTop = 0x%16lx\n", pageTop);
	Console::WriteFormat("PAGE_TOP_LEVEL = 0x%16lx\n", PAGE_TOP_LEVEL);
	Console::WriteFormat("PAGE_TOP_REC = 0x%16lx\n", PAGE_TOP_REC);
*/
	memtotal = membelow4g = (info->UpperMemory + 1024) * 1024;
	pagestotal = pagesbelow4g = membelow4g >> PAGE_SIZE_SHIFT;
	allocendvirt = (unsigned long)tabBITS + (((pagesbelow4g >> 3) + PAGE_SIZE - 1) & (~PAGE_SIZE_MASK));
	allocendphys = allocendvirt - Symbol::kernelOffset.Addr();

	basemem.bitmap = tabBITS;
	basemem.length = membelow4g;
	basemem.prev = basemem.next = &basemem;

	// Mark free and allocated areas.
	basemem.Mark(false);
	basemem.Mark(0x00000000, 0x00001000, true); // Real Mode IVT + BIOS Data Area
	basemem.Mark(0x0009f000, 0x000a0000, true); // Extended BIOS Data Area
	basemem.Mark(0x000a0000, 0x00100000, true); // Video RAM + BIOS ROM
	basemem.Mark(0x00100000, allocendphys, true); // Kernel image

	// Unmap any previously mapped memory above the kernel.
	Unmap((void*)allocendvirt, PAGES_PER_TABLE - (allocendphys >> PAGE_SIZE_SHIFT));

	// Mark Multiboot info and associated structures as allocated.
	basemem.Mark((unsigned long)mbi, (unsigned long)mbi + sizeof(Multiboot::Info), true);
	info = (Multiboot::Info*)MapToLinear(mbi, (void*)((PAGE_SIZE_MASK & (unsigned long)mbi) + allocendvirt), 2);

	if((info->Flags & Multiboot::FLAGS_DRIVES) && (info->DrivesLength > 0))
		basemem.Mark(info->DrivesAddress, info->DrivesAddress + info->DrivesLength, true);

	if(info->Flags & Multiboot::FLAGS_MEMORY_MAP)
		basemem.Mark(info->MemoryMapAddress, info->MemoryMapAddress + info->MemoryMapLength, true);

	if(info->Flags & Multiboot::FLAGS_NAME)
		basemem.Mark(info->BootLoaderName, info->BootLoaderName, true);

	if(info->Flags & Multiboot::FLAGS_CMDLINE)
		basemem.Mark(info->CommandLine, info->CommandLine, true);

	if((info->Flags & Multiboot::FLAGS_MODULES) && (info->ModuleCount > 0))
	{
		basemem.Mark(info->ModuleAddress, info->ModuleAddress + info->ModuleCount * sizeof(Multiboot::Module), true);
		mmod = (Multiboot::Module*)MapToLinear((void*)(unsigned long)(info->ModuleAddress), (void*)((PAGE_SIZE_MASK & info->ModuleAddress) + allocendvirt + 2 * PAGE_SIZE), 2);
		basemem.Mark(mmod[0].ModStart, mmod[info->ModuleCount - 1].ModEnd, true);
		Unmap(mmod, 2);
	}

	Unmap(info, 2);

	// Map user trampoline.
	MapToLinear((void*)(Symbol::userStart.Addr() - Symbol::kernelOffset.Addr()), Symbol::libraryStart.Ptr(), (Symbol::userEnd.Addr() - Symbol::userStart.Addr()) / PAGE_SIZE);

	Console::WriteMessage(Console::Style::OK, "Pager:", "Enabled, %d kB RAM", membelow4g >> 10);
}

unsigned long X86Pager::GetBlockSize(void)
{
	return(PAGE_SIZE);
}

bool X86Pager::PageExists(unsigned long n)
{
//	Console::WriteFormat("PageExists(0x%8x) - top: %s\n", n, (IsTopPage(n) ? "yes" : "no"));
	return((IsTopPage(n) || PageExists((n >> PAGE_BITS) + ((unsigned long)PAGE_RECURSIVE << (PAGE_BITS * (TABLE_LEVELS - 1))))) && Pages(n).bits.present);
}

bool X86Pager::VirtExists(void* addr)
{
	return(PageExists(((unsigned long)addr >> PAGE_SIZE_SHIFT) & (PAGE_COUNT - 1)));
}

unsigned long X86Pager::VirtToPhys(void* addr)
{
	if(!VirtExists(addr))
		return(~0UL);
	return(Pages(((unsigned long)addr >> PAGE_SIZE_SHIFT) & (PAGE_COUNT - 1)).bits.page << PAGE_SIZE_SHIFT | ((unsigned long)addr & PAGE_SIZE_MASK));
}

bool X86Pager::TableExists(unsigned long n)
{
	return(PageExists(n + ((unsigned long)PAGE_RECURSIVE << (PAGE_BITS * (TABLE_LEVELS - 1)))));
}

bool X86Pager::TableEmpty(unsigned long n)
{
	unsigned int i;

	if(!TableExists(n))
		return false;

	for(i = 0; i < PAGES_PER_TABLE; i++)
		if(PageTables(n).page[i].raw != 0)
			return false;

	return true;
}

void X86Pager::CreateTable(unsigned long n)
{
	unsigned int i;

	AllocBlocks(&PageTables(n), 1);
	for(i = 0; i < PAGES_PER_TABLE; i++)
		PageTables(n).page[i].raw = 0;
}

void X86Pager::DestroyTable(unsigned long n)
{
	unsigned int i;

//	Console::WriteFormat("DestroyTable(0x%8x)\n", n);

	if(!TableExists(n))
		return;

	for(i = 0; i < PAGES_PER_TABLE; i++)
	{
		if(PageTables(n).page[i].raw)
		{
			if(n >> (PAGE_BITS * (TABLE_LEVELS - 2)) == PAGE_RECURSIVE)
				DestroyTable(((n - ((pageDir - pageTab) >> PAGE_SIZE_SHIFT)) << PAGE_BITS) + i);
			Mark(PageTables(n).page[i].bits.page << PAGE_SIZE_SHIFT, false);
			PageTables(n).page[i].raw = 0;
		}
	}
}

void* X86Pager::MapToLinear(void* phys, void* virt, unsigned long n)
{
	unsigned long i, src, dst, ptr, v;
	Process* p = &kprocess();

//	Console::WriteFormat("MapToLinear(0x%16lx, 0x%16lx, 0x%16lx)\n", phys, virt, n);

	for(i = 0; i < n; i++)
	{
		v = ((unsigned long)virt & ~PAGE_SIZE_MASK) + (i << PAGE_SIZE_SHIFT);
		src = ((unsigned long)phys >> PAGE_SIZE_SHIFT) + i;
		dst = (((unsigned long)virt >> PAGE_SIZE_SHIFT) & (PAGE_COUNT - 1)) + i;

		if(!TableExists(dst / PAGES_PER_TABLE))
			CreateTable(dst / PAGES_PER_TABLE);

		PageTableEntry& page = Pages(dst);
		page.raw = src << PAGE_SIZE_SHIFT;
		page.bits.present = 1;
		page.bits.global = (v >= Symbol::libraryStart.Addr()) && !IsTablePage(dst);

		if(dst != PAGE_TOP_REC)
		{
			ptr = dst;
			while(IsTablePage(ptr))
				ptr = (ptr - (pageDir - pageTab) / sizeof(long)) << PAGE_BITS;

			page.bits.user = (ptr < ((Symbol::supervisorStart.Addr() >> PAGE_SIZE_SHIFT) & (PAGE_COUNT - 1)));
			page.bits.writeable = (ptr < ((Symbol::libraryStart.Addr() >> PAGE_SIZE_SHIFT) & (PAGE_COUNT - 1)));
		}

		if(IsTopPage(dst) && ((dst - PAGE_TOP_LEVEL) * TABLE_COUNT >= ((Symbol::libraryStart.Addr() >> PAGE_SIZE_SHIFT) & (PAGE_COUNT - 1))))
		{
			do
			{
				p->data.pgtab->page[dst - PAGE_TOP_LEVEL].raw = page.raw;
				p = p->next;
			}
			while(p != &kprocess());
		}

		InvalidatePage(v);
	}
	return(virt);
}

void* X86Pager::AllocBlocks(void* virt, unsigned long n, unsigned long first, unsigned long last, bool cont)
{
	MemoryArea* ma = &basemem;
	unsigned long i, phys;

	i = 0;
	do
	{
		ma = ma->prev;

		if(cont)
		{
			phys = ma->AllocBlocks(n, first, last);
			if(phys)
				return(MapToLinear((void*)phys, virt, n));
		}
		else
		{
			while(true)
			{
				phys = ma->AllocBlocks(1, first, last);
				if(!phys)
					break;
				i++;
				MapToLinear((void*)phys, (void*)((unsigned long)virt + ((n - i) << PAGE_SIZE_SHIFT)), 1);
				if(i == n)
					return virt;
			}
		}
	} while(ma != &basemem);

	if(i > 0)
		FreeBlocks((void*)((unsigned long)virt + ((n - i) << PAGE_SIZE_SHIFT)), i);
	return(nullptr);
}

void* X86Pager::AllocBlocks(void* virt, unsigned long n)
{
	return(AllocBlocks(virt, n, 0UL, ~0UL, false));
}

void X86Pager::FreeBlocks(void* virt, unsigned long n)
{
	unsigned long i;
	unsigned long dst = ((unsigned long)virt >> PAGE_SIZE_SHIFT) & (PAGE_COUNT - 1);
	unsigned long src;

//	Console::WriteFormat("FreeBlocks(0x%16lx, 0x%16lx)\n", virt, n);

	for(i = 0; i < n; i++)
	{
		if(Pages(dst + i).raw != 0)
		{
			src = Pages(dst + i).raw & ~PAGE_SIZE_MASK;
			Mark(src, false);
		}
	}

	Unmap(virt, n);
}

void X86Pager::Mark(unsigned long phys, bool alloc)
{
	MemoryArea* ma = &basemem;
	do
	{
		ma = ma->prev;
		if((phys >= ma->start) && (phys < ma->start + ma->length))
		{
			ma->Mark((phys - ma->start) >> PAGE_SIZE_SHIFT, alloc);
			break;
		}
	} while(ma != &basemem);
}

void X86Pager::Unmap(void* virt, unsigned long n)
{
	unsigned long i;
	unsigned long dst, addr;
	Process* p = &kprocess();

//	Console::WriteFormat("Unmap(0x%16lx, 0x%16lx)\n", virt, n);

	for(i = 0; i < n; i++)
	{
		addr = (unsigned long)virt + (i << PAGE_SIZE_SHIFT);
		dst = (addr >> PAGE_SIZE_SHIFT) & (PAGE_COUNT - 1);
		if(TableExists(dst >> PAGE_BITS))
		{
			Pages(dst).raw = 0;
			if(IsTopPage(dst) && ((dst - PAGE_TOP_LEVEL) * TABLE_COUNT >= ((Symbol::libraryStart.Addr() >> PAGE_SIZE_SHIFT) & (PAGE_COUNT - 1))))
			{
				do
				{
					p->data.pgtab->page[dst - PAGE_TOP_LEVEL].raw = 0;
					p = p->next;
				}
				while(p != &kprocess());
			}
			InvalidatePage(addr);
		}
	}

	dst = ((unsigned long)virt >> PAGE_SIZE_SHIFT) & (PAGE_COUNT - 1);
	for(i = dst / PAGES_PER_TABLE; i <= (dst + n) / PAGES_PER_TABLE; i++)
		if(TableEmpty(i))
			FreeBlocks(&PageTables(i), 1);
}

void* X86Pager::MapTempRegion(unsigned long start, unsigned long length)
{
	unsigned long first, last;
	void* virt;

	first = start >> PAGE_SIZE_SHIFT;
	last = (start + length - 1) >> PAGE_SIZE_SHIFT;
	virt = MapToLinear((void*)(first << PAGE_SIZE_SHIFT), (void*)allocendvirt, last - first + 1);
	allocendvirt += (last - first + 1) << PAGE_SIZE_SHIFT;

	return((void*)((start & PAGE_SIZE_MASK) + (unsigned long)virt));
}

void* X86Pager::MapTempString(unsigned long start)
{
	char* ret = (char*)MapToLinear((void*)(start & ~PAGE_SIZE_MASK), (void*)allocendvirt, 1) + (start & PAGE_SIZE_MASK);
	char* p = ret;

	allocendvirt += PAGE_SIZE;

	while(true)
	{
		do
		{
			if(!(*(p++)))
				return(ret);
		}
		while((PAGE_SIZE_MASK & (unsigned long)p) != 0);

		MapToLinear((void*)((p - ret) + start), p, 1);
		allocendvirt += PAGE_SIZE;
	}
}

void X86Pager::AddMemoryArea(Multiboot::MemoryMap* mem)
{
	MemoryArea* ma;

	if(mem->Type == 1)
	{
		if(mem->BaseAddr + mem->Length <= membelow4g)
			return;

		if(mem->BaseAddr > -1UL)
			return;

		ma = new MemoryArea(mem->BaseAddr, mem->Length, new unsigned long[mem->Length / (PAGE_SIZE * sizeof(long) * 8)]);
		ma->Mark(false);
		memtotal += mem->Length;
		pagestotal += (mem->Length) >> PAGE_SIZE_SHIFT;
	}
	else
	{
		ma = new MemoryArea(mem->BaseAddr, mem->Length, nullptr);
		ma->offset = (mem->Length < (16 * 1024 * 1024) ? (unsigned long)MapTempRegion(mem->BaseAddr, mem->Length) - mem->BaseAddr : 0);
	}

	ma->prev = basemem.prev;
	ma->next = &basemem;
	basemem.prev->next = ma;
	basemem.prev = ma;
}

void X86Pager::CleanInit(void)
{
	unsigned long n;
	void* virt;

	n = (Symbol::initEnd.Addr() - Symbol::initStart.Addr()) >> PAGE_SIZE_SHIFT;
	virt = Symbol::initStart.Ptr();

	Console::WriteFormat("Free %d blocks at 0x%p\n", n, virt);
	FreeBlocks(virt, n);

	Console::WriteMessage(Console::Style::INFO, "Total memory:", "%u MB", memtotal >> 20);
}

void* X86Pager::PhysToVirt(unsigned long addr)
{
	MemoryArea* ma = &basemem;
	do
	{
		ma = ma->prev;
		if((addr >= ma->start) && (addr < ma->start + ma->length) && (ma->bitmap == nullptr))
			return((void*)(ma->offset + addr));
	} while(ma != &basemem);
	return((void*)-1);
}

X86Pager::PageTable* X86Pager::CreateContext(void)
{
	PageTable* pg;
	unsigned long phys, i;

	pg = (PageTable*)(Heap::Alloc(PAGE_SIZE, PAGE_SIZE, true));

	for(i = 0; i < ((Symbol::libraryStart.Addr() >> PAGE_SIZE_SHIFT) & (PAGE_COUNT - 1)) / TABLE_COUNT; i++)
		pg->page[i].raw = 0;

	for(; i < PAGES_PER_TABLE; i++)
		pg->page[i].raw = TopPages(i).raw;

	phys = VirtToPhys(pg);
	pg->page[PAGE_RECURSIVE].raw = phys;
	pg->page[PAGE_RECURSIVE].bits.present = 1;

	return(pg);
}

unsigned long X86Pager::SwitchContext(unsigned long phys)
{
	unsigned long old;

	old = CR3::Read();
	CR3::Write((old & PAGE_SIZE_MASK) | (unsigned long)phys);

	return(old & ~PAGE_SIZE_MASK);
}

void X86Pager::DeleteContext(PageTable* pg)
{
	unsigned long i, cr3;

	cr3 = CR3::Read();
	CR3::Write(VirtToPhys(pg));
	for(i = 0; i < ((Symbol::libraryStart.Addr() >> PAGE_SIZE_SHIFT) & (PAGE_COUNT - 1)) / TABLE_COUNT; i++)
	{
		if(TopPages(i).raw)
		{
			DestroyTable(((pageTop - pageDir) >> (PAGE_SIZE_SHIFT - PAGE_BITS)) + i);
		}
	}
	CR3::Write(cr3);
	Heap::Free(pg);
}
