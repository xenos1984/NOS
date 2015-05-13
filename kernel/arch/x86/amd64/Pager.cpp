// Pager.cpp - Working with page tables

#include INC_SUBARCH(Pager.h)
#include INC_SUBARCH(PMode.h)
#include <Console.h>
#include <Memory.h>
#include <VirtualMemory.h>
#include INC_ARCH(ControlRegisters.h)
#include INC_ARCH(Multiboot.h)
#include INC_SUBARCH(X86_64Process.h)

#define CHECK_EXISTS(addr) (void*)(VirtExists((void*)(addr)) ? (addr) : (-1))

using namespace Kernel;

// char physmem_space[sizeof(Pager)];

SECTION(".init.text") Pager::Pager(MultibootInfo* mbi)
{
	unsigned long i;
	MBMemoryMap* mmap;
	MBMemoryMap* mmap0;
	MBModule* mmod;
	MultibootInfo* info = (MultibootInfo*)((unsigned long)mbi + (unsigned long)&kernelOffset);

	bitmap = tabBITS;
	totalmem = (info->UpperMemory + 1024) * 1024;
	bmlength = totalmem >> (PAGE_SIZE_SHIFT + 6);
	numpages = totalmem >> PAGE_SIZE_SHIFT;
	allocend = (unsigned long)bitmap + ((8 * bmlength + PAGE_SIZE - 1) & (~PAGE_SIZE_MASK));
	allocendphys = allocend - (unsigned long)&kernelOffset;
//	console().WriteFormat("Mem: 0x%8x, BML: 0x%8x, NP: 0x%8x, AE: 0x%8x\n", totalmem, bmlength, numpages, allocendphys);

	// Set virtual location for paging structures.
	pml4t = (PageTable*)0xfffffffffffff000;
	pgdip = (PageTable*)0xffffffffffe00000;
	pgdir = (PageTable*)0xffffffffc0000000;
	pgtab = (PageTable*)0xffffff8000000000;

	// Map only used pages (and area below 1MB) into virtual memory.
	for(i = (allocendphys >> PAGE_SIZE_SHIFT); i <= 511; i++)
		pgtab[0x6000000].page[i] = 0;

	// Mark used pages as allocated
	for(i = 0; i < allocendphys >> (PAGE_SIZE_SHIFT + 6); i++)
		bitmap[i] = 0xffffffffffffffff;
	bitmap[allocendphys >> (PAGE_SIZE_SHIFT + 6)] = 0xffffffffffffffff >> (64 - ((allocendphys >> PAGE_SIZE_SHIFT) & 63));
	for(i = (allocendphys >> (PAGE_SIZE_SHIFT + 6)) + 1; i < bmlength; i++)
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

	console().WriteMessage(Console::MSG_OK, "Pager:", "Enabled @ 0x%16lx, memtop @ 0x%16lx", VirtToPhys(pml4t), (((totalmem - 1) >> 20) + 1) << 20);
/*	console().WriteFormat("PML4T = %16lx => %16lx\n", pml4t, VirtToPhys(pml4t));
	console().WriteFormat("PGDIP = %16lx => %16lx\n", pgdip, VirtToPhys(pgdip));
	console().WriteFormat("PGDIR = %16lx => %16lx\n", pgdir, VirtToPhys(pgdir));
	console().WriteFormat("PGTAB = %16lx => %16lx\n", pgtab, VirtToPhys(pgtab));*/
}

unsigned long Pager::GetBlockSize(void)
{
	return(PAGE_SIZE);
}

bool Pager::VirtExists(void* addr)
{
	unsigned long page = ((unsigned long)addr >> PAGE_SIZE_SHIFT) & 0x0000000fffffffff;
	if(!TableExists(page >> 9))
		return(false);
	return((bool)(((unsigned long*)pgtab)[page] & PAGE_PRESENT));
}

unsigned long Pager::VirtToPhys(void* addr)
{
	unsigned long page = ((unsigned long)addr >> PAGE_SIZE_SHIFT) & 0x0000000fffffffff, offs = (unsigned long)addr & PAGE_SIZE_MASK;
	if(!TableExists(page >> 9))
		return(~0UL);
	if(!(((unsigned long*)pgtab)[page] & PAGE_PRESENT))
		return(~0UL);
	return((((unsigned long*)pgtab)[page] & ~PAGE_SIZE_MASK) + offs);
}

void* Pager::PhysToVirt(unsigned long addr)
{
	console().WriteFormat("PhysToVirt(0x%16x)\n", addr);
	if(addr < 0x100000)
		return(CHECK_EXISTS(addr + (unsigned long)&kernelOffset));
	else if(addr < 0x100000000)
		return(CHECK_EXISTS(addr + 0xffffcfff00000000));

	console().WriteFormat("No virtual mapping for address 0x%16x.\n", addr);
	return((void*)(-1));
}

void* Pager::MapToDefault(void* first, void* last)
{
	unsigned long start, end;

	start = (unsigned long)first & (~PAGE_SIZE_MASK);
	end = ((unsigned long)last + PAGE_SIZE - 1) & (~PAGE_SIZE_MASK);

	if((unsigned long)end <= 0x100000)
		return((void*)((unsigned long)first + (unsigned long)&kernelOffset));

	if((unsigned long)end < 0x100000000)
	{
		MapToLinear((void*)start, (void*)(start + 0xffffcfff00000000), (end - start) >> PAGE_SIZE_SHIFT);
		return((void*)((unsigned long)first + 0xffffcfff00000000));
	}

	console().WriteFormat("No default map for area 0x%16x - 0x%16x.\n");
	return((void*)(-1));
}

void* Pager::MapToLinear(void* phys, void* virt, unsigned long n)
{
	unsigned long i;
	unsigned long src = (unsigned long)phys >> PAGE_SIZE_SHIFT;
	unsigned long dst = ((unsigned long)virt >> PAGE_SIZE_SHIFT) & 0x0000000fffffffff;
	unsigned long v;
	X86_64Process* p = &x86_64kprocess();

	for(i = 0; i < n; i++)
	{
//		console().WriteFormat("Mapping 0x%16lx to 0x%16lx\n", src, dst);
		v = ((unsigned long)virt & ~PAGE_SIZE_MASK) + (i << PAGE_SIZE_SHIFT);
		if(!TableExists((dst + i) >> 9))
			CreateTable((dst + i) >> 9);
		((unsigned long*)pgtab)[dst + i] = ((src + i) << PAGE_SIZE_SHIFT) | PAGE_PRESENT
			| (
				(v < (unsigned long)&userOffset) ||
				((v >= (unsigned long)pgtab) && (v < ((unsigned long)pgtab + (((unsigned long)&userOffset >> 9) & 0x7fffffffff)))) ||
				((v >= (unsigned long)pgdir) && (v < ((unsigned long)pgdir + (((unsigned long)&userOffset >> 18) & 0x3fffffff)))) ||
				((v >= (unsigned long)pgdip) && (v < ((unsigned long)pgdip + (((unsigned long)&userOffset >> 27) & 0x1fffff))))
			? PAGE_READ_WRITE : 0)
			| (
				(v < (unsigned long)&kernelOffset) ||
				((v >= (unsigned long)pgtab) && (v < ((unsigned long)pgtab + (((unsigned long)&kernelOffset >> 9) & 0x7fffffffff)))) ||
				((v >= (unsigned long)pgdir) && (v < ((unsigned long)pgdir + (((unsigned long)&kernelOffset >> 18) & 0x3fffffff)))) ||
				((v >= (unsigned long)pgdip) && (v < ((unsigned long)pgdip + (((unsigned long)&kernelOffset >> 27) & 0x1fffff))))
			? PAGE_P3_ACCESS : 0)
			| (
				(v < (unsigned long)&kernelOffset) && (v < (unsigned long)pgtab)
			? PAGE_GLOBAL : 0);
		if((v >= 0xfffffffffff00000) && (v < 0xfffffffffffff000))
		{
//			console().WriteFormat("Kernel directory mapping: 0x%16lx = entry %d\n", v, dst + i - 0xffffffe00);
			do
			{
				p->pml4t->page[dst + i - 0xffffffe00] = ((unsigned long*)pgtab)[dst + i];
				p = (X86_64Process*)p->next;
			}
			while(p != &x86_64kprocess());
		}
		InvalidatePage(v);
//		console().WriteFormat("Page 0x%16lx => 0x%16lx\n", virt, pgtab->page[dst + i]);
	}

	return(virt);
}

void* Pager::AllocBlocks(void* virt, unsigned long n, unsigned char area, bool cont)
{
	unsigned long i, k;
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

//	console().WriteFormat("AllocBlocks(0x%16x, 0x%16x)\n", (unsigned long)virt, n);

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
	unsigned long i;
	unsigned long dst = ((unsigned long)virt >> PAGE_SIZE_SHIFT) & 0x0000000fffffffff;
	unsigned long src;

	for(i = 0; i < n; i++)
	{
		if(pgtab->page[dst + i] != 0)
		{
			src = pgtab->page[dst + i] >> PAGE_SIZE_SHIFT;
//			console().WriteFormat("Unmapping 0x%16x from 0x%16x\n", src, dst);
			MarkFree(src);
			pgtab->page[dst + i] = 0;
			InvalidatePage((unsigned long)virt + (i << PAGE_SIZE_SHIFT));
		}
	}

	for(i = dst >> 9; i <= (dst + n) >> 9; i++)
		if(TableEmpty(i))
			FreeBlocks(&(pgtab[i]), 1);
}

void Pager::Unmap(void* virt, unsigned long n)
{
	unsigned long i;
	unsigned long dst = ((unsigned long)virt >> PAGE_SIZE_SHIFT) & 0x0000000fffffffff;
	X86_64Process* p = &x86_64kprocess();

	for(i = 0; i < n; i++)
	{
		((unsigned long*)pgtab)[dst + i] = 0;
		if((dst + i >= 0xfffffff00) && (dst + i < 0xfffffffff))
		{
			do
			{
				p->pml4t->page[dst + i - 0xffffffe00] = 0;
				p = (X86_64Process*)p->next;
			}
			while(p != &x86_64kprocess());
		}
	}

	for(i = dst >> 9; i <= (dst + n) >> 9; i++)
		if(TableEmpty(i))
			FreeBlocks(&(pgtab[i]), 1);
}

void Pager::CreateTable(unsigned long n)
{
	unsigned long i;

	n &= 0x0000000007ffffff;
//	console().WriteFormat("CreateTable(0x%16x)\n", n);
	AllocBlocks(&(pgtab[n]), 1, AREA_ARBITRARY, false);
	for(i = 0; i<= 511; i++)
		pgtab[n].page[i] = 0;
}

bool Pager::IsAlloc(unsigned long page)
{
//	console().WriteFormat("IsAlloc(0x%16x) = 0x%16x\n", page, bitmap[page >> 6] >> (page & 63));
	return((bitmap[page >> 6] >> (page & 63)) & 1UL);
}

void Pager::MarkAlloc(unsigned long page)
{
	bitmap[page >> 6] |= (1UL << (page & 63));
//	console().WriteFormat("Alloc page 0x%8x.\n", page);
}

void Pager::MarkFree(unsigned long page)
{
	bitmap[page >> 6] &= ~(1UL << (page & 63));
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

bool Pager::DirPointerExists(unsigned long n)
{
//	console().WriteFormat("DirPointerExists(0x%16x)\n", n);
	return(pml4t->page[n] != 0);
}

bool Pager::DirExists(unsigned long n)
{
//	console().WriteFormat("DirExists(0x%16x)\n", n);
	if(!DirPointerExists(n >> 9))
		return(false);
	return(pgdip->page[n] != 0);
}

bool Pager::TableExists(unsigned long n)
{
	n &= 0x0000000007ffffff;
//	console().WriteFormat("TableExists(0x%16x)\n", n);
	if(!DirExists(n >> 9))
		return(false);
	return(pgdir->page[n] != 0);
}

bool Pager::DirPointerEmpty(unsigned long n)
{
	unsigned long i;

	if(!DirPointerExists(n))
		return(false);

	for(i = 0; i <= 511; i++)
	{
		if(pgdip[n].page[i] != 0)
			return(false);
	}

	return(true);
}

bool Pager::DirEmpty(unsigned long n)
{
	unsigned long i;

	if(!DirExists(n))
		return(false);

	for(i = 0; i <= 511; i++)
	{
		if(pgdir[n].page[i] != 0)
			return(false);
	}

	return(true);
}

bool Pager::TableEmpty(unsigned long n)
{
	unsigned long i;

	n &= 0x0000000007ffffff;
	if(!TableExists(n))
		return(false);

	for(i = 0; i <= 511; i++)
	{
		if(pgtab[n].page[i] != 0)
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

	for(i = 0; i < 256; i++)
		pg->page[i] = 0;

	for(; i <= 510; i++)
		pg->page[i] = pml4t->page[i];

	phys = pgtab->page[((unsigned long)pg >> PAGE_SIZE_SHIFT) & 0x0000000fffffffff] & (~PAGE_SIZE_MASK);
	pg->page[511] = (unsigned long)phys | PAGE_PRESENT | PAGE_READ_WRITE;

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

	char* copy = (char*)(virtmem().Alloc(5 * PAGE_SIZE, PAGE_SIZE, false));
	PageTable* pml = (PageTable*)(&copy[PAGE_SIZE]);
	PageTable* ptr = (PageTable*)(&copy[2 * PAGE_SIZE]);
	PageTable* dir = (PageTable*)(&copy[3 * PAGE_SIZE]);
	PageTable* tab = (PageTable*)(&copy[4 * PAGE_SIZE]);

	ctx = CR3::Read();
	CR3::Write((phys & ~PAGE_SIZE_MASK) | (ctx & PAGE_SIZE_MASK));

	MapToLinear((void*)(ctx & ~PAGE_SIZE_MASK), pml, 1);
	MapToLinear((void*)(pml->page[(((unsigned long)src) >> 39) & 0x1ff] & ~PAGE_SIZE_MASK), ptr, 1);
	MapToLinear((void*)(ptr->page[(((unsigned long)src) >> 30) & 0x1ff] & ~PAGE_SIZE_MASK), dir, 1);
	MapToLinear((void*)(dir->page[(((unsigned long)src) >> 21) & 0x1ff] & ~PAGE_SIZE_MASK), tab, 1);
	MapToLinear((void*)(tab->page[(((unsigned long)src) >> 12) & 0x1ff] & ~PAGE_SIZE_MASK), copy, 1);

	if(!VirtExists(dst))
		AllocBlocks((void*)(((unsigned long)dst) & ~PAGE_SIZE_MASK), 1);

	for(i = 0; i < length; i++)
	{
		if(((i + (unsigned long)dst) & PAGE_SIZE_MASK) == 0 && !VirtExists((void*)(i + (unsigned long)dst)))
			AllocBlocks((void*)(i + (unsigned long)dst), 1);

		if(((i + (unsigned long)src) & 0x7fffffffff) == 0)
			MapToLinear((void*)(pml->page[((i + (unsigned long)src) >> 39) & 0x1ff] & ~PAGE_SIZE_MASK), ptr, 1);

		if(((i + (unsigned long)src) & 0x3fffffff) == 0)
			MapToLinear((void*)(ptr->page[((i + (unsigned long)src) >> 30) & 0x1ff] & ~PAGE_SIZE_MASK), dir, 1);

		if(((i + (unsigned long)src) & 0x1fffff) == 0)
			MapToLinear((void*)(dir->page[((i + (unsigned long)src) >> 21) & 0x1ff] & ~PAGE_SIZE_MASK), tab, 1);

		if(((i + (unsigned long)src) & PAGE_SIZE_MASK) == 0)
			MapToLinear((void*)(tab->page[((i + (unsigned long)src) >> 12) & 0x1ff] & ~PAGE_SIZE_MASK), copy, 1);

		((char*)dst)[i] = copy[(i + (unsigned long)src) & PAGE_SIZE_MASK];
	}

	Unmap(copy, 5);
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
	unsigned long i, j, k, l, cr3;

	cr3 = CR3::Read();
	CR3::Write(VirtToPhys(pg));
	for(i = 0; i < 256; i++)
	{
		if(pml4t->page[i] & PAGE_PRESENT)
		{
			for(j = 0; j < 512; j++)
			{
				if(pgdip[i].page[j] & PAGE_PRESENT)
				{
					for(k = 0; k < 512; k++)
					{
						if(pgdir[(i << 9) + j].page[k] & PAGE_PRESENT)
						{
							for(l = 0; l < 512; l++)
							{
								if(pgtab[(i << 18) + (j << 9) + k].page[l] & PAGE_PRESENT)
								{
									MarkFree(pgtab[(i << 18) + (j << 9) + k].page[l] >> PAGE_SIZE_SHIFT);
								}
							}
							MarkFree(pgdir[(i << 9) + j].page[k] >> PAGE_SIZE_SHIFT);
						}
					}
					MarkFree(pgdip[i].page[j] >> PAGE_SIZE_SHIFT);
				}
			}
			MarkFree(pml4t->page[i] >> PAGE_SIZE_SHIFT);
		}
	}
	CR3::Write(cr3);
	virtmem().Free(pg);
}
