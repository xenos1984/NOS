// I386TaskManager.cpp - Switching tasks in a multiprocessor environment.

#include <new>
#include <cstddef>
#include INC_SUBARCH(I386TaskManager.h)
#include <Thread.h>
#include <Process.h>
#include INC_ARCH(Processor.h)
#include INC_ARCH(ControlRegisters.h)
#include INC_ARCH(Apic.h)
#include INC_ARCH(CPU.h)
#include INC_ARCH(FPU.h)
#include INC_SUBARCH(PMode.h)
#include INC_SUBARCH(Entry.h)
#include INC_ARCH(DescriptorTable.h)
#include INC_ARCH(Interrupt.h)
#include INC_ARCH(X86Pager.h)
#include INC_ARCH(IOApic.h)
#include INC_ARCH(Cmos.h)
#include INC_ARCH(ACPI.h)
#include INC_ARCH(SMP.h)
#include INC_ARCH(PICManager.h)
#include INC_ARCH(IOApicManager.h)
#include <Symbol.h>
#include <TaskScheduler.h>
#include <Console.h>

using namespace Kernel;

char taskman_space[sizeof(I386TaskManager)];
extern "C" unsigned long bspStack;

SECTION(".init.text") I386TaskManager::I386TaskManager(unsigned int nc)
{
	unsigned long i;
	TSS* tss = (TSS*)TSS_LIN_ADDR;

	numbuses = numioapics = numints = numlints = 0;
	numcpus =  nc;

	cpu = (Processor*)new char[numcpus * sizeof(Processor)];

	new (tasksched_space) TaskScheduler(numcpus);

	for(i = 0; i < numcpus; i++)
	{
		tss = (TSS*)(physmem().AllocBlocks((void*)(TSS_LIN_ADDR + i * TSS_LENGTH), 1));
		tss->iobase = 0x1000;
		tabGDT.CreateTSS(FIRST_TSS + i, (void*)(TSS_LIN_ADDR + i * TSS_LENGTH), 0x1000); // TSS_LENGTH);
	}
	new (irqman_space) PICManager();
}

void SECTION(".init.text") I386TaskManager::Init(void)
{
	Console::WriteMessage(Console::Style::INFO, "CPU mode:", "single CPU");

	new (taskman_space) I386TaskManager(1);

	new (i386taskman().cpu) Processor;
	i386taskman().bsp = i386taskman().cpu;
	SetTaskReg(FIRST_TSS << 3);
	physmem().MapToLinear((void*)((unsigned long)&bspStack - Symbol::kernelOffset.Addr() - STACK_SIZE), (void*)STACK_LIN_ADDR, STACK_SIZE / 0x1000);
	asm volatile("addl %0, %%esp" : : "r"(STACK_LIN_ADDR + STACK_SIZE - (unsigned long)&bspStack));

	if(bspcpu().HasAPIC())
	{
		Apic::Init();
		Apic::SetTimerVector(0x40);
		Apic::SetTimerDiv(Apic::TDR_64);
		Apic::TimerStart(0x10000);
	}
	else
	{
		Console::WriteMessage(Console::Style::WARNING, "APIC:", "Not present.");
		cmos().SetPeriodic(7); // 512Hz
		irqman().Unmask(8);
	}
}

#ifdef CONFIG_ACPI
void SECTION(".init.text") I386TaskManager::InitAcpi(void)
{
	unsigned long i;
	Processor* pr;
	ACPI::LApic* ala;

	if(ACPI::GetProcessorCount() == 0)
	{
		Init();
		return;
	}

	new (taskman_space) I386TaskManager(ACPI::GetProcessorCount());
	IOApicManager::InitAcpi();

	for(i = 0; i < i386taskman().numcpus; i++)
	{
		ala = ACPI::GetProcessor(i);
		new (&(i386taskman().cpu[i])) Processor(ala);
		pr = &(i386taskman().cpu[i]);
		if(i == 0) // Hey, it's me!
		{
			i386taskman().bsp = pr;
			SetTaskReg((FIRST_TSS + i) << 3);
			physmem().MapToLinear((void*)((unsigned long)&bspStack - Symbol::kernelOffset.Addr() - STACK_SIZE), (void*)STACK_LIN_ADDR, STACK_SIZE / 0x1000);
			asm volatile("addl %0, %%esp" : : "r"(STACK_LIN_ADDR + STACK_SIZE - (unsigned long)&bspStack));
		}
		else if(ala->Flags & ACPI::CPU_ENABLED)
			pr->Startup(STACK_SIZE + (unsigned long)(physmem().AllocBlocks((void*)(STACK_LIN_ADDR + i * STACK_SIZE), STACK_SIZE / 0x1000)));
	}

	Apic::SetTimerVector(0x40);
	Apic::SetTimerDiv(Apic::TDR_64);
	Apic::TimerStart(0x10000);
}
#endif

#ifdef CONFIG_SMP
void SECTION(".init.text") I386TaskManager::InitSmp(void)
{
	unsigned long i;
	Processor* pr;
	SMP::Cpu* sc;

	new (taskman_space) I386TaskManager(smp().GetProcessorCount());
	IOApicManager::InitSmp();

	for(i = 0; i < i386taskman().numcpus; i++)
	{
		sc = smp().GetProcessor(i);
		new (&(i386taskman().cpu[i])) Processor(sc);
		pr = &(i386taskman().cpu[i]);
		if(sc->Flags & SMP::CPU_BSP) // Hey, it's me!
		{
			i386taskman().bsp = pr;
			SetTaskReg((FIRST_TSS + i) << 3);
			physmem().MapToLinear((void*)((unsigned long)&bspStack - Symbol::kernelOffset.Addr() - STACK_SIZE), (void*)(STACK_LIN_ADDR + i * STACK_SIZE), STACK_SIZE / 0x1000);
			asm volatile("addl %0, %%esp" : : "r"(STACK_LIN_ADDR + (i + 1) * STACK_SIZE - (unsigned long)&bspStack));
		}
		else if(sc->Flags & SMP::CPU_ENABLED)
			pr->Startup(STACK_SIZE + (unsigned long)(physmem().AllocBlocks((void*)(STACK_LIN_ADDR + i * STACK_SIZE), STACK_SIZE / 0x1000)));
	}

	Apic::SetTimerVector(0x40);
	Apic::SetTimerDiv(Apic::TDR_64);
	Apic::TimerStart(0x10000);
}
#endif

int I386TaskManager::GetCurrentCPU(void)
{
	return((GetTaskReg() >> 3) - FIRST_TSS);
}

Processor* I386TaskManager::GetProcessor(unsigned char n)
{
	return(&cpu[n]);
}

void SECTION(".init.text") I386TaskManager::SetTSS(void)
{
	unsigned char id;
	int i;

	if(numcpus == 1)
		return;
	id = Apic::GetPhysID();
	for(i = 0; i < numcpus; i++)
	{
		if(cpu[i].GetPhysID() == id)
		{
			SetTaskReg((FIRST_TSS + i) << 3);
			return;
		}
	}
}

int strcmp(const char* s1, const char* s2)
{
	while (*s1 != 0 && *s1 == *s2)
	{
		s1++;
		s2++;
	}

	return (*(unsigned char*)s1) - (*(unsigned char*)s2);
}

Process* I386TaskManager::CreateProcess(Elf* elf)
{
	Process* p;
	unsigned long context, base, length;

	unsigned int j, k, m;
	unsigned char rtype;
	unsigned int rsym;

	Elf::ProgramHeader* ph = elf->GetProgramHeader();
	Elf::SectionHeader* sh = elf->GetSectionHeader();
	Elf::Relocation* rel;
//	Elf::RelocationA* rela;
	Elf::Symbol* sym;
	char* str;

	p = new Process;
	p->data.pgtab = x86pager().CreateContext();
	p->data.cr3 = x86pager().VirtToPhys(p->data.pgtab);
	context = x86pager().SwitchContext(p->data.cr3);

	for(j = 0; j < elf->GetHeader()->PHNum; j++)
	{
		if(ph[j].Type != Elf::PT_LOAD)
			continue;

		base = (ph[j].VirtAddress / X86Pager::PAGE_SIZE) * X86Pager::PAGE_SIZE;
		length = (ph[j].MemSize + (ph[j].VirtAddress - base) - 1) / X86Pager::PAGE_SIZE + 1;

		for(k = 0; k < length; k++)
		{
			if(!physmem().VirtExists((void*)(base + k * X86Pager::PAGE_SIZE)))
			{
				physmem().AllocBlocks((void*)(base + k * X86Pager::PAGE_SIZE), 1);
				p->memory.FetchAndAdd(X86Pager::PAGE_SIZE);
			}
		}

		for(k = 0; k < ph[j].FileSize; k++)
			((unsigned char*)(ph[j].VirtAddress))[k] = ((unsigned char*)((unsigned long)elf + ph[j].Offset))[k];

		for(; k < ph[j].MemSize; k++)
			((unsigned char*)(ph[j].VirtAddress))[k] = 0;
	}

	for(j = 0; j < elf->GetHeader()->SHNum; j++)
	{
		if((sh[j].Type == Elf::SHT_REL) && (sh[sh[j].Link].Type == Elf::SHT_DYNSYM))
		{
			rel = (Elf::Relocation*)((unsigned long)elf + sh[j].Offset);
			sym = (Elf::Symbol*)((unsigned long)elf + sh[sh[j].Link].Offset);
			str = (char*)((unsigned long)elf + sh[sh[sh[j].Link].Link].Offset);

			for(k = 0; k < sh[j].Size / sh[j].EntrySize; k++)
			{
				rtype = (unsigned char)rel[k].Info;
				rsym = rel[k].Info >> 8;

				switch(rtype)
				{
				case 7: // R_386_JMP_SLOT
					for(m = 0; m < (((unsigned long)kernelElfDynstr) - ((unsigned long)kernelElfDynsym)) / sizeof(Elf::Symbol); m++)
					{
						if(!strcmp(&str[sym[rsym].Name], &kernelElfDynstr[kernelElfDynsym[m].Name]))
						{
							*(unsigned long*)(rel[k].Offset) = USER_OFFSET + (m << 4);
							break;
						}
					}
					break;

				default:
					break;
				}
			}
		}
		/* else if((sh[j].Type == Elf::SHT_RELA) && (sh[sh[j].Link].Type == Elf::SHT_DYNSYM))
		{
			rela = (Elf::RelocationA*)((unsigned long)elf + sh[j].Offset);
			sym = (Elf::Symbol*)((unsigned long)elf + sh[sh[j].Link].Offset);
			str = (char*)((unsigned long)elf + sh[sh[sh[j].Link].Link].Offset);
		} */
	}

	x86pager().SwitchContext(context);

	tasksched().CreateProcess(p);
	CreateThread(p, (void*)elf->GetHeader()->Entry, (void*)0);

	return(p);
}

Thread* I386TaskManager::CreateThread(Process* p, void* entry, void* stack)
{
	Thread* t = new Thread(p);

	tasksched().CreateThread(t);
	t->data.eip = (unsigned long)entry;
	t->data.eflags = 0x00000202;
	t->data.esp = (unsigned long)stack;
	t->data.mmstate = nullptr;

	return(t);
}

void I386TaskManager::DeleteThread(Thread* t)
{
	if(t->data.mmstate)
		delete[] t->data.mmstate;
	delete t;
}

void I386TaskManager::DeleteProcess(Process* p)
{
	x86pager().DeleteContext(p->data.pgtab);
	delete p;
}

void I386TaskManager::SwitchTo(Thread* from, Thread* to)
{
	unsigned long cr0;
	ThreadData** stacktop = (ThreadData**)(STACK_LIN_ADDR + (GetCurrentCPU() + 1) * STACK_SIZE - 4);

	// If the FPU / SSE has been used, save its contents.
	if((from != nullptr) && (from->data.mmstate != nullptr))
	{
		cr0 = CR0::Read();
		if((cr0 & CR0::TASK_SWITCHED) == 0)
			from->data.mmstate->Save();
		CR0::Write(cr0 | CR0::TASK_SWITCHED);
	}

	// Change pointer to register save area and CR3 if necessary.
	if(to == nullptr)
	{
		*stacktop = nullptr;
		if(from != nullptr)
			CR3::Write(kprocess().data.cr3);
	}
	else
	{
		*stacktop = &(to->data);
		if((from == nullptr) || (from->owner != to->owner))
			CR3::Write(to->owner->data.cr3);
	}
}
