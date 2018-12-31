// Interrupt.h - Interrupts and exceptions in ARM architecture.

#include INC_BITS(Interrupt.h)
#include INC_BITS(SystemRegs.h)
//#include INC_SUBARCH(PageTable.h)
#include <Console.h>

namespace Kernel
{
	void DumpExceptionSyndrome(uint32_t syn)
	{
		unsigned int cls = syn >> 26;
		unsigned int iss = syn & ((1 << 25) - 1);

		switch(cls)
		{
		case 0x01:
			Console::WriteFormat("Trapped WFI or WFE instruction execution.\n");
			break;

		case 0x03:
		case 0x05:
			Console::WriteFormat("Trapped MCR or MRC access.\n");
			break;

		case 0x04:
		case 0x0c:
			Console::WriteFormat("Trapped MCRR or MRRC access.\n");
			break;

		case 0x06:
			Console::WriteFormat("Trapped LDC or STC access.\n");
			break;

		case 0x07:
			Console::WriteFormat("Trapped access to SVE, Advanced SIMD, or floating-point functionality.\n");
			break;

		case 0x08:
			Console::WriteFormat("Trapped VMRS access, from ID group trap.\n");
			break;

		case 0x09:
			Console::WriteFormat("Trapped Pointer Authentication instruction.\n");
			break;

		case 0x0e:
			Console::WriteFormat("Illegal Execution state.\n");
			break;

		case 0x11:
			Console::WriteFormat("SVC instruction execution in AArch32 state.\n");
			break;

		case 0x12:
			Console::WriteFormat("HVC instruction execution in AArch32 state, when HVC is not disabled.\n");
			break;

		case 0x13:
			Console::WriteFormat("SMC instruction execution in AArch32 state, when SMC is not disabled.\n");
			break;

		case 0x15:
			Console::WriteFormat("SVC instruction execution in AArch64 state.\n");
			break;

		case 0x16:
			Console::WriteFormat("HVC instruction execution in AArch64 state, when HVC is not disabled.\n");
			break;

		case 0x17:
			Console::WriteFormat("SMC instruction execution in AArch64 state, when SMC is not disabled.\n");
			break;

		case 0x18:
			Console::WriteFormat("Trapped MSR, MRS or System instruction execution in AArch64 state.\n");
			break;

		case 0x19:
			Console::WriteFormat("Trapped access to SVE functionality.\n");
			break;

		case 0x1a:
			Console::WriteFormat("Trapped ERET, ERETAA, or ERETAB instruction execution.\n");
			break;

		case 0x1f:
			Console::WriteFormat("Implementation defined exception to EL3.\n");
			break;

		case 0x20:
		case 0x21:
			Console::WriteFormat("Instruction Abort %s.\n", (cls & 0x1 ? "taken without a change in Exception level" : "from a lower Exception level, that might be using AArch32 or AArch64"));
			break;

		case 0x22:
			Console::WriteFormat("PC alignment fault exception.\n");
			break;

		case 0x24:
		case 0x25:
			{
				Console::WriteFormat("Data Abort %s.\n", (cls & 0x1 ? "taken without a change in Exception level" : "from a lower Exception level, that might be using AArch32 or AArch64"));
				unsigned int dfsc = iss & 0x3f;
				switch(dfsc)
				{
				case 0x00:
				case 0x01:
				case 0x02:
				case 0x03:
					Console::WriteFormat("Address size fault at level %d.\n", dfsc);
					break;

				case 0x04:
				case 0x05:
				case 0x06:
				case 0x07:
					Console::WriteFormat("Translation fault at level %d.\n", dfsc & 0x3);
					break;

				case 0x08:
				case 0x09:
				case 0x0a:
				case 0x0b:
					Console::WriteFormat("Access flag fault at level %d.\n", dfsc & 0x3);
					break;

				case 0x0c:
				case 0x0d:
				case 0x0e:
				case 0x0f:
					Console::WriteFormat("Permission fault at level %d.\n", dfsc & 0x3);
					break;

				case 0x10:
					Console::WriteFormat("Synchronous External abort, not on translation table walk.\n");
					break;

				case 0x14:
				case 0x15:
				case 0x16:
				case 0x17:
					Console::WriteFormat("Synchronous External abort, on translation table walk at level %d.\n", dfsc & 0x3);
					break;

				case 0x18:
					Console::WriteFormat("Synchronous parity or ECC error on memory access, not on translation table walk.\n");
					break;

				case 0x1c:
				case 0x1d:
				case 0x1e:
				case 0x1f:
					Console::WriteFormat("Synchronous parity or ECC error on memory access on translation table walk at level %d.\n", dfsc & 0x3);
					break;

				case 0x21:
					Console::WriteFormat("Alignment fault.\n");
					break;

				case 0x30:
					Console::WriteFormat("TLB conflict abort.\n");
					break;

				case 0x31:
					Console::WriteFormat("Unsupported atomic hardware update fault.\n");
					break;

				case 0x34:
					Console::WriteFormat("Implementation defined fault (Lockdown).\n");
					break;

				case 0x35:
					Console::WriteFormat("Implementation defined fault (Unsupported Exclusive or Atomic access).\n");
					break;

				case 0x3d:
					Console::WriteFormat("Section Domain Fault.\n");
					break;

				case 0x3e:
					Console::WriteFormat("Page Domain Fault.\n");
					break;

				default:
					Console::WriteFormat("Unknown Data Fault Status Code 0x%2x.\n", dfsc);
					break;
				}
			}
			break;

		case 0x26:
			Console::WriteFormat("SP alignment fault exception.\n");
			break;

		case 0x28:
			Console::WriteFormat("Trapped floating-point exception taken from AArch32 state.\n");
			break;

		case 0x2c:
			Console::WriteFormat("Trapped floating-point exception taken from AArch64 state.\n");
			break;

		case 0x2f:
			Console::WriteFormat("SError interrupt.\n");
			break;

		case 0x30:
		case 0x31:
			Console::WriteFormat("Breakpoint exception %s.\n", (cls & 0x1 ? "taken without a change in Exception level" : "from a lower Exception level, that might be using AArch32 or AArch64"));
			break;

		case 0x32:
		case 0x33:
			Console::WriteFormat("Software Step exception %s.\n", (cls & 0x1 ? "taken without a change in Exception level" : "from a lower Exception level, that might be using AArch32 or AArch64"));
			break;

		case 0x34:
		case 0x35:
			Console::WriteFormat("Watchpoint exception %s.\n", (cls & 0x1 ? "taken without a change in Exception level" : "from a lower Exception level, that might be using AArch32 or AArch64"));
			break;

		case 0x38:
			Console::WriteFormat("BKPT instruction execution in AArch32 state.\n");
			break;

		case 0x3a:
			Console::WriteFormat("Vector Catch exception from AArch32 state.\n");
			break;

		case 0x3c:
			Console::WriteFormat("BRK instruction execution in AArch64 state.\n");
			break;

		default:
			Console::WriteFormat("Unknown reason 0x%2x.\n", cls);
			break;
		}
	}
}

using namespace Kernel;

void RegisterSet::Dump()
{
	Console::WriteFormat(" r0 = 0x%16lx,  r1 = 0x%16lx,  r2 = 0x%16lx,\n", r0, r1, r2);
	Console::WriteFormat(" r3 = 0x%16lx,  r4 = 0x%16lx,  r5 = 0x%16lx,\n", r3, r4, r5);
	Console::WriteFormat(" r6 = 0x%16lx,  r7 = 0x%16lx,  r8 = 0x%16lx,\n", r6, r7, r8);
	Console::WriteFormat(" r9 = 0x%16lx, r10 = 0x%16lx, r11 = 0x%16lx,\n", r9, r10, r11);
	Console::WriteFormat("r12 = 0x%16lx, r13 = 0x%16lx, r14 = 0x%16lx,\n", r12, r13, r14);
	Console::WriteFormat("r15 = 0x%16lx, r16 = 0x%16lx, r17 = 0x%16lx,\n", r15, r16, r17);
	Console::WriteFormat("r18 = 0x%16lx, r19 = 0x%16lx, r20 = 0x%16lx,\n", r18, r19, r20);
	Console::WriteFormat("r21 = 0x%16lx, r22 = 0x%16lx, r23 = 0x%16lx,\n", r21, r22, r23);
	Console::WriteFormat("r24 = 0x%16lx, r25 = 0x%16lx, r26 = 0x%16lx,\n", r24, r25, r26);
	Console::WriteFormat("r27 = 0x%16lx, r28 = 0x%16lx, r29 = 0x%16lx,\n", r27, r28, r29);
	Console::WriteFormat(" lr = 0x%16lx,  sp = 0x%16lx.\n", lr, sp);
}

extern "C" void KernelSyncHandler(RegisterSet* regs)
{
	uint64_t syndrome = Sysreg::ESR_EL1::Read();
	uint64_t pc = Sysreg::ELR_EL1::Read();
	uint64_t ps = Sysreg::SPSR_EL1::Read();

	Console::WriteFormat("Synchronous exception occurred with ESR = 0x%8x:\n", syndrome);
	DumpExceptionSyndrome(syndrome);
	Console::WriteFormat(" pc = 0x%16lx,  ps = 0x%16lx.\n", pc, ps);
	regs->Dump();
}
