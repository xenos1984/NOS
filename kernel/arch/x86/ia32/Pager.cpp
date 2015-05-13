// Pager.cpp - Working with page tables

#include INC_SUBARCH(Pager.h)
#include INC_SUBARCH(PMode.h)
#include <Console.h>
#include <Memory.h>
#include <VirtualMemory.h>
#include INC_ARCH(Multiboot.h)
#include INC_ARCH(ControlRegisters.h)
#include INC_SUBARCH(I386Process.h)

#define CHECK_EXISTS(addr) (void*)(VirtExists((void*)(addr)) ? (addr) : (-1))

using namespace Kernel;

// char physmem_space[sizeof(Pager)];

SECTION(".init.text") Pager::Pager(MultibootInfo* mbi)
{
	unsigned int i;
	MBMemoryMap* mmap;
	MBMemoryMap* mmap0;
	MBModule* mmod;
	MultibootInfo* info = (MultibootInfo*)((unsigned long)mbi + (unsigned long)&kernelOffset);

	bitmap = tabBITS;
	totalmem = (info->UpperMemory + 1024) * 1024;
	bmlength = totalmem >> (PAGE_SIZE_SHIFT + 5);
	numpages = totalmem >> PAGE_SIZE_SHIFT;
	allocend = (unsigned long)bitmap + ((4 * bmlength + PAGE_SIZE - 1) & (~PAGE_SIZE_MASK));
	allocendphys = allocend - (unsigned long)&kernelOffset;
	memtop = (((totalmem - 1) >> 20) + 1) << 20;

	// Set physical and virtual location for page directory and first page table.
	pgdirvirt = (PageTable*)0xfffff000;
	pgtabvirt = (PageTable*)0xffc00000;
	pgdirphys = &tabPGDIR;
	pgtabphys = &tabPGTAB;

	// Map only used pages (and area below 1MB) into virtual memory.
	for(i = (allocendphys >> PAGE_SIZE_SHIFT); i <= 1023; i++)
		pgtabvirt[0x300].page[i] = 0;

	// Mark used pages as allocated
	for(i = 0; i < allocendphys >> (PAGE_SIZE_SHIFT + 5); i++)
		bitmap[i] = 0xffffffff;
	bitmap[allocendphys >> (PAGE_SIZE_SHIFT + 5)] = 0xffffffff >> (32 - ((allocendphys >> PAGE_SIZE_SHIFT) & 31));
	for(i = (allocendphys >> (PAGE_SIZE_SHIFT + 5)) + 1; i < bmlength; i++)
		bitmap[i] = 0;

	MapToLinear((void*)((unsigned long)&userStart - (unsigned long)&kernelOffset), (void*)&userOffset, ((unsigned long)&userEnd - (unsigned long)&userStart) / PAGE_SIZE);

	// Map Multiboot info
	info = (MultibootInfo*)MapToDefault(mbi, (void*)((unsigned long)mbi + sizeof(MultibootInfo)));
	MarkAlloc(mbi, (void*)((unsigned long)mbi + sizeof(MultibootInfo)));

	// Map MMIO / ACPI areas
	if(info->Flags & Multiboot::MB_FLAGS_MEMORY_MAP)
	{
		mmap0 = (MBMemoryMap*)MapToDefault((void*)(info->MemoryMapAddress), (void*)(info->MemoryMapAddress + info->MemoryMapLength * sizeof(MBMemoryMap)));
		MarkAlloc((void*)(info->MemoryMapAddress), (void*)(info->MemoryMapAddress + info->MemoryMapLength * sizeof(MBMemoryMap)));
		for(mmap = mmap0; (unsigned long)mmap - (unsigned long)mmap0 < info->MemoryMapLength; mmap = (MBMemoryMap*)((unsigned long)mmap + mmap->Size + 4))
		{
			if(mmap->Type > 1)
			{
				MapToDefault((void*)(mmap->BaseAddr), (void*)(mmap->BaseAddr + mmap->Length));
			}
		}
	}

	if((info->Flags & Multiboot::MB_FLAGS_DRIVES) && (info->DrivesLength > 0))
	{
		MapToDefault((void*)(info->DrivesAddress), (void*)(info->DrivesAddress + info->DrivesLength * sizeof(MBDrive)));
		MarkAlloc((void*)(info->DrivesAddress), (void*)(info->DrivesAddress + info->DrivesLength * sizeof(MBDrive)));
	}

	// Map modules
	if((info->Flags & Multiboot::MB_FLAGS_MODULES) && (info->ModuleCount > 0))
	{
		mmod = (MBModule*)MapToDefault((void*)(info->ModuleAddress), (void*)(info->ModuleAddress + info->ModuleCount * sizeof(MBModule)));
		MapToDefault((void*)(mmod[0].ModStart), (void*)(mmod[info->ModuleCount - 1].ModEnd));
		MarkAlloc((void*)(info->ModuleAddress), (void*)(info->ModuleAddress + info->ModuleCount * sizeof(MBModule)));
		MarkAlloc((void*)(mmod[0].ModStart), (void*)(mmod[info->ModuleCount - 1].ModEnd));
		console().WriteMessage(Console::MSG_INFO, "Modules:", "0x%8x to 0x%8x", mmod[0].ModStart, mmod[info->ModuleCount - 1].ModEnd);
	}

	console().WriteMessage(Console::MSG_OK, "Pager:", "Enabled @ 0x%8x, memtop @ 0x%8x", (unsigned long)pgdirphys, memtop);
}

unsigned long Pager::GetBlockSize(void)
{
	return(PAGE_SIZE);
}

bool Pager::VirtExists(void* addr)
{
	unsigned long page = (unsigned long)addr >> PAGE_SIZE_SHIFT;
	if(!TableExists(page >> 10))
		return(false);
	return((bool)(((unsigned long*)pgtabvirt)[page] & PAGE_PRESENT));
}

unsigned long Pager::VirtToPhys(void* addr)
{
	unsigned long page = (unsigned long)addr >> PAGE_SIZE_SHIFT, offs = (unsigned long)addr & PAGE_SIZE_MASK;
	if(!TableExists(page >> 10))
		return(~0UL);
	if(!(((unsigned long*)pgtabvirt)[page] & PAGE_PRESENT))
		return(~0UL);
	return((((unsigned long*)pgtabvirt)[page] & ~PAGE_SIZE_MASK) + offs);
}

void* Pager::PhysToVirt(unsigned long addr)
{
	if(addr < 0x1000000)
		return(CHECK_EXISTS(addr + 0xc0000000));
	else if(addr >= 0xf0000000)
		return(CHECK_EXISTS(addr - 0x10000000));
	else if((addr < memtop) && (addr + 0x08000000 >= memtop))
		return(CHECK_EXISTS(addr - memtop + 0xd0000000));
	return((void*)(-1));
}

void* Pager::MapToDefault(void* first, void* last)
{
	unsigned long start, end;

	start = (unsigned long)first & (~PAGE_SIZE_MASK);
	end = ((unsigned long)last + PAGE_SIZE - 1) & (~PAGE_SIZE_MASK);

	if((unsigned long)end <= 0x100000)
		return((void*)((unsigned long)first + (unsigned long)&kernelOffset));

	if((unsigned long)end < 0x1000000)
	{
		MapToLinear((void*)start, (void*)(start + (unsigned long)&kernelOffset), (end - start) >> PAGE_SIZE_SHIFT);
		return((void*)((unsigned long)first + (unsigned long)&kernelOffset));
	}

	if(start >= 0xf0000000)
	{
		MapToLinear((void*)start, (void*)(start - 0x10000000), (end - start) >> PAGE_SIZE_SHIFT);
		return((void*)((unsigned long)first - 0x10000000));
	}

	if((start - memtop + 0xd0000000 >= 0xc8000000) && (end <= memtop))
	{
		MapToLinear((void*)start, (void*)(start - memtop + 0xd0000000), (end - start) >> PAGE_SIZE_SHIFT);
		return((void*)((unsigned long)first - memtop + 0xd0000000));
	}
	return((void*)(-1));
}

void* Pager::MapToLinear(void* phys, void* virt, unsigned long n)
{
	unsigned int i;
	unsigned long src = (unsigned long)phys >> PAGE_SIZE_SHIFT, dst = (unsigned long)virt >> PAGE_SIZE_SHIFT;
	unsigned long v;
	I386Process* p = &i386kprocess();

//	console().WriteFormat("Mapping 0x%8x to 0x%8x\n", phys, virt);
	for(i = 0; i < n; i++)
	{
		v = ((unsigned long)virt & ~PAGE_SIZE_MASK) + (i << PAGE_SIZE_SHIFT);
		if(!TableExists((dst + i) >> 10))
			CreateTable((dst + i) >> 10);
		((unsigned long*)pgtabvirt)[dst + i] = ((src + i) << PAGE_SIZE_SHIFT) | PAGE_PRESENT
			| (
				(v < (unsigned long)&userOffset) ||
				((v >= (unsigned long)pgtabvirt) && (v < (unsigned long)pgtabvirt + ((unsigned long)&userOffset >> 10)))
			? PAGE_READ_WRITE : 0)
			| (
				(v < (unsigned long)&kernelOffset) ||
				((v >= (unsigned long)pgtabvirt) && (v < (unsigned long)pgtabvirt + ((unsigned long)&kernelOffset >> 10)))
			? PAGE_P3_ACCESS : 0)
			| (
				(v >= (unsigned long)&kernelOffset) && (v < (unsigned long)pgtabvirt)
			? PAGE_GLOBAL : 0);
		if((v >= 0xffe00000) && (v < 0xfffff000))
		{
//			console().WriteFormat("Kernel directory mapping: 0x%8x = entry %d\n", v, dst + i - 0xffc00);
			do
			{
				p->pgdir->page[dst + i - 0xffc00] = ((unsigned long*)pgtabvirt)[dst + i];
				p = (I386Process*)p->next;
			}
			while(p != &i386kprocess());
		}
		InvalidatePage(v);
	}
	return(virt);
}

void* Pager::AllocBlocks(void* virt, unsigned long n, unsigned char area, bool cont)
{
	unsigned int i;
	unsigned long k;
	bool ok;

	switch(area)
	{
	case AREA_BELOW_1MB:
		i = 255;
		break;
	case AREA_BELOW_16MB:
		i = 4095;
		break;
	default:
		i = numpages - 1;
	}

	if(cont)
	{
		for(; i >= n - 1; i--)
		{
			if(!IsAlloc(i))
			{
				ok = true;
				for(k = 1; k < n; k++)
				{
					if(IsAlloc(i - k))
					{
						ok = false;
						break;
					}
				}
				if(ok)
				{
					for(k = 0; k < n; k++)
						MarkAlloc(i - k);
					MapToLinear((void*)((i - n + 1) << PAGE_SIZE_SHIFT), virt, n);
					return(virt);
				}
			}
		}
	}
	else
	{
		k = n;
		do
		{
			if(!IsAlloc(i))
			{
				k--;
				MarkAlloc(i);
				MapToLinear((void*)(i << PAGE_SIZE_SHIFT), (void*)((unsigned long)virt + (k << PAGE_SIZE_SHIFT)), 1);
				if(k == 0)
					return(virt);
			}
		}
		while(i-- > 0);
		if(k < n)
			FreeBlocks((void*)((unsigned long)virt + (k << PAGE_SIZE_SHIFT)), n - k);
	}
	return(nullptr);
}

void* Pager::AllocBlocks(void* virt, unsigned long n)
{
	return AllocBlocks(virt, n, AREA_ARBITRARY, false);
}

void Pager::FreeBlocks(void* virt, unsigned long n)
{
	unsigned int i;
	unsigned long dst = (unsigned long)virt >> PAGE_SIZE_SHIFT;
	unsigned long src;

	for(i = 0; i < n; i++)
	{
		if(((unsigned long*)pgtabvirt)[dst + i] != 0)
		{
			src = ((unsigned long*)pgtabvirt)[dst + i] >> PAGE_SIZE_SHIFT;
			// console().WriteFormat("Unmapping 0x%8x from 0x%8x\n", src, dst);
			MarkFree(src);
			((unsigned long*)pgtabvirt)[dst + i] = 0;
			InvalidatePage((dst + i) << PAGE_SIZE_SHIFT);
		}
	}

	for(i = dst >> 10; i <= (dst + n) >> 10; i++)
		if(TableEmpty(i))
			FreeBlocks(&(pgtabvirt[i]), 1);
}

void Pager::Unmap(void* virt, unsigned long n)
{
	unsigned long i;
	unsigned long dst = (unsigned long)virt >> PAGE_SIZE_SHIFT;
	I386Process* p = &i386kprocess();

	for(i = 0; i < n; i++)
	{
		((unsigned long*)pgtabvirt)[dst + i] = 0;
		if((dst + i >= 0xffe00) && (dst + i < 0xfffff))
		{
			do
			{
				p->pgdir->page[dst + i - 0xffc00] = 0;
				p = (I386Process*)p->next;
			}
			while(p != &i386kprocess());
		}
	}

	for(i = dst >> 10; i <= (dst + n) >> 10; i++)
		if(TableEmpty(i))
			FreeBlocks(&(pgtabvirt[i]), 1);
}

void Pager::CreateTable(unsigned long num)
{
	int i;

	AllocBlocks(&(pgtabvirt[num]), 1, AREA_ARBITRARY, false);
	for(i = 0; i<= 1023; i++)
		pgtabvirt[num].page[i] = 0;
}

bool Pager::IsAlloc(unsigned long page)
{
	return((bitmap[page >> 5] & (1 << (page & 31))) != 0);
}

void Pager::MarkAlloc(unsigned long page)
{
	bitmap[page >> 5] |= (1 << (page & 31));
//	console().WriteFormat("Alloc page 0x%8x.\n", page);
}

void Pager::MarkFree(unsigned long page)
{
	bitmap[page >> 5] &= ~(1 << (page & 31));
//	console().WriteFormat("Free page 0x%8x.\n", page);
}

void Pager::MarkAlloc(void* first, void* last)
{
	unsigned long i;

	for(i = ((unsigned long)first) >> PAGE_SIZE_SHIFT; i <= ((unsigned long)last) >> PAGE_SIZE_SHIFT; i++)
		MarkAlloc(i);
}

void Pager::MarkFree(void* first, void* last)
{
	unsigned long i;

	for(i = ((unsigned long)first) >> PAGE_SIZE_SHIFT; i <= ((unsigned long)last) >> PAGE_SIZE_SHIFT; i++)
		MarkFree(i);
}

bool Pager::TableExists(unsigned long tab)
{
	return(pgdirvirt->page[tab] != 0);
}

bool Pager::TableEmpty(unsigned long tab)
{
	int i;

	if(!TableExists(tab))
		return(false);

	for(i = 0; i <= 1023; i++)
	{
		if(pgtabvirt[tab].page[i] != 0)
			return(false);
	}

	return(true);
}

PageTable* Pager::CreateContext(void)
{
	PageTable* pg;
	unsigned long phys;
	int i;

	pg = (PageTable*)(virtmem().Alloc(PAGE_SIZE, PAGE_SIZE));

	for(i = 0; (i << 22) < (unsigned long)&userOffset; i++)
		pg->page[i] = 0;

	for(; i <= 1022; i++)
		pg->page[i] = pgdirvirt->page[i];

	phys = ((unsigned long*)pgtabvirt)[((unsigned long)pg) >> PAGE_SIZE_SHIFT] & (~PAGE_SIZE_MASK);
	pg->page[1023] = (unsigned long)phys | PAGE_PRESENT | PAGE_READ_WRITE;

	return(pg);
}

unsigned long Pager::SwitchContext(unsigned long phys)
{
	unsigned long old;

	old = CR3::Read();
	CR3::Write((old & PAGE_SIZE_MASK) | (unsigned long)phys);

	return(old & ~PAGE_SIZE_MASK);
}

void Pager::CopyToContext(unsigned long phys, void* src, void* dst, unsigned long length)
{
	unsigned long ctx, i;

	char* copy = (char*)(virtmem().Alloc(3 * PAGE_SIZE, PAGE_SIZE, false));
	PageTable* dir = (PageTable*)(&copy[PAGE_SIZE]);
	PageTable* tab = (PageTable*)(&copy[2 * PAGE_SIZE]);

	ctx = CR3::Read();
	CR3::Write((phys & ~PAGE_SIZE_MASK) | (ctx & PAGE_SIZE_MASK));

	MapToLinear((void*)(ctx & ~PAGE_SIZE_MASK), dir, 1);
	MapToLinear((void*)(dir->page[((unsigned long)src) >> 22] & ~PAGE_SIZE_MASK), tab, 1);
	MapToLinear((void*)(tab->page[(((unsigned long)src) >> 12) & 0x3ff] & ~PAGE_SIZE_MASK), copy, 1);

	if(!VirtExists(dst))
		AllocBlocks((void*)(((unsigned long)dst) & ~PAGE_SIZE_MASK), 1);

	for(i = 0; i < length; i++)
	{
		if(((i + (unsigned long)dst) & PAGE_SIZE_MASK) == 0 && !VirtExists((void*)(i + (unsigned long)dst)))
			AllocBlocks((void*)(i + (unsigned long)dst), 1);

		if(((i + (unsigned long)src) & 0x3fffff) == 0)
			MapToLinear((void*)(dir->page[(i + (unsigned long)src) >> 22] & ~PAGE_SIZE_MASK), tab, 1);

		if(((i + (unsigned long)src) & PAGE_SIZE_MASK) == 0)
			MapToLinear((void*)(tab->page[((i + (unsigned long)src) >> 12) & 0x3ff] & ~PAGE_SIZE_MASK), copy, 1);

		((char*)dst)[i] = copy[(i + (unsigned long)src) & PAGE_SIZE_MASK];
	}

	Unmap(copy, 3);
	virtmem().Free(copy, false);
	CR3::Write(ctx);
}

void Pager::FillContext(unsigned long phys, unsigned char fill, void* dst, unsigned long length)
{
	unsigned long ctx, i;

	ctx = CR3::Read();
	CR3::Write((phys & ~PAGE_SIZE_MASK) | (ctx & PAGE_SIZE_MASK));

	if(!VirtExists(dst))
		AllocBlocks((void*)(((unsigned long)dst) & ~PAGE_SIZE_MASK), 1);

	for(i = 0; i < length; i++)
	{
		if(((i + (unsigned long)dst) & PAGE_SIZE_MASK) == 0 && !VirtExists((void*)(i + (unsigned long)dst)))
			AllocBlocks((void*)(i + (unsigned long)dst), 1);

		((char*)dst)[i] = fill;
	}

	CR3::Write(ctx);
}

void Pager::DeleteContext(PageTable* pg)
{
	unsigned long i, j, cr3;

	cr3 = CR3::Read();
	CR3::Write(VirtToPhys(pg));
	for(i = 0; i < 512; i++)
	{
		if(pgdirvirt->page[i] & PAGE_PRESENT)
		{
			for(j = 0; j < 1024; j++)
			{
				if(pgtabvirt[i].page[j] & PAGE_PRESENT)
				{
					MarkFree(pgtabvirt[i].page[j] >> PAGE_SIZE_SHIFT);
				}
			}
			MarkFree(pgdirvirt->page[i] >> PAGE_SIZE_SHIFT);
		}
	}
	CR3::Write(cr3);
	virtmem().Free(pg);
}
