// Interrupt.h - Interrupts and exceptions in ARM architecture.

#include INC_BITS(Interrupt.h)
#include INC_BITS(SystemRegs.h)
#include INC_SUBARCH(PageTable.h)
#include <Console.h>

namespace Kernel
{
	void DumpFaultStatus(uint32_t status)
	{
		Console::WriteFormat((status & 0x1000 ? "External abort; " : "Internal abort; "));
		Console::WriteFormat((status & 0x800 ? "write; " : "read; "));

		switch(((status & 0x400) >> 6) | (status & 0xf))
		{
		case 1:
			Console::WriteFormat("alignment fault.\n");
			break;
		case 2:
			Console::WriteFormat("debug event.\n");
			break;
		case 3:
			Console::WriteFormat("access flag fault (PT1).\n");
			break;
		case 4:
			Console::WriteFormat("instruction cache maintenance.\n");
			break;
		case 5:
			Console::WriteFormat("translation fault (PT1).\n");
			break;
		case 6:
			Console::WriteFormat("access flag fault (PT2).\n");
			break;
		case 7:
			Console::WriteFormat("translation fault (PT2).\n");
			break;
		case 8:
			Console::WriteFormat("sync. external abort.\n");
			break;
		case 9:
			Console::WriteFormat("domain fault (PT1).\n");
			break;
		case 11:
			Console::WriteFormat("domain fault (PT2).\n");
			break;
		case 12:
			Console::WriteFormat("sync. ext. abort on ttbl walk (PT1).\n");
			break;
		case 13:
			Console::WriteFormat("permission fault (PT2).\n");
			break;
		case 14:
			Console::WriteFormat("sync. ext. abort on ttbl walk (PT2).\n");
			break;
		case 15:
			Console::WriteFormat("permission fault (PT1).\n");
			break;
		case 16:
			Console::WriteFormat("TLB conflict abort.\n");
			break;
		case 20:
			Console::WriteFormat("lockdown.\n");
			break;
		case 22:
			Console::WriteFormat("async. external abort.\n");
			break;
		case 24:
			Console::WriteFormat("async. parity error on mem. access.\n");
			break;
		case 25:
			Console::WriteFormat("sync. parity error on mem. access.\n");
			break;
		case 26:
			Console::WriteFormat("coprocessor abort.\n");
			break;
		case 28:
			Console::WriteFormat("sync. parity error on ttbl walk (PT1).\n");
			break;
		case 30:
			Console::WriteFormat("sync. parity error on ttbl walk (PT2).\n");
			break;
		default:
			Console::WriteFormat("unknown abort.\n");
		}
	}

	void DumpFaultAddr(uintptr_t addr)
	{
		Console::WriteFormat("Faulting page: %d / %d\n", addr >> Memory::PGB_1M, (addr >> Memory::PGB_4K) & 0xff);

		Pager::PageTableEntryL1& pte1 = Pager::PageTableTop().Entry(addr >> Memory::PGB_1M);
		Console::WriteFormat("Page level 1 @ 0x%8x: 0x%8x\n", &pte1, pte1.Raw());
		if(!pte1.IsFault())
		{
			Pager::PageTableEntryL2& pte2 = Pager::PageTableL2::Table(addr >> Memory::PGB_1M).Entry((addr >> Memory::PGB_4K) & 0xff);
			Console::WriteFormat("Page level 2 @ 0x%8x: 0x%8x\n", &pte2, pte2.Raw());
		}
	}
}

using namespace Kernel;

void RegisterSet::Dump()
{
	Console::WriteFormat(" r0 = 0x%8x,  r1 = 0x%8x,  r2 = 0x%8x,  r3 = 0x%8x,\n", r0, r1, r2, r3);
	Console::WriteFormat(" r4 = 0x%8x,  r5 = 0x%8x,  r6 = 0x%8x,  r7 = 0x%8x,\n", r4, r5, r6, r7);
	Console::WriteFormat(" r8 = 0x%8x,  r9 = 0x%8x, r10 = 0x%8x, r11 = 0x%8x,\n", r8, r9, r10, r11);
	Console::WriteFormat("r12 = 0x%8x, usp = 0x%8x, ulr = 0x%8x,  pc = 0x%8x,\n", r12, usp, ulr, pc);
	Console::WriteFormat(" lr = 0x%8x, psr = 0x%8x.\n", lr, spsr);
}

extern "C" void PrefetchAbort(RegisterSet* regs)
{
	uint32_t ifsr = Sysreg::IFSR::Read();
	uint32_t ifar = Sysreg::IFAR::Read();

	Console::WriteFormat("Prefetch abort occurred\n");
	Console::WriteFormat("IFAR = 0x%8x, IFSR = 0x%8x\n", ifar, ifsr);
	DumpFaultStatus(ifsr);
	DumpFaultAddr(ifar);
	regs->Dump();
}

extern "C" void DataAbort(RegisterSet* regs)
{
	uint32_t dfsr = Sysreg::DFSR::Read();
	uint32_t dfar = Sysreg::DFAR::Read();

	Console::WriteFormat("Data abort occurred\n");
	Console::WriteFormat("DFAR = 0x%8x, DFSR = 0x%8x\n", dfar, dfsr);
	DumpFaultStatus(dfsr);
	DumpFaultAddr(dfar);
	regs->Dump();
}
