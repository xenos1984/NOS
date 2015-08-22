// AML.cpp - ACPI Machine Language

#include INC_ARCH(ACPI.h)
#include INC_ARCH(AML.h)
#include <Console.h>

#define WAIT asm volatile ( \
"1: \n" \
"	inb $0x64, %%al \n" \
"	test $0x01, %%al \n" \
"	jz 2f \n" \
"	inb $0x60, %%al \n" \
"	jmp 1b \n" \
"2: \n" \
"	inb $0x64, %%al \n" \
"	test $0x01, %%al \n" \
"	jz 2b":::"eax");

using namespace Kernel;

#ifdef CONFIG_ACPI
void SECTION(".init.text") AML::ParseTable(ACPI::TableHeader* header)
{
/*	unsigned char* ptr = (unsigned char*)(sizeof(AcpiHeader) + (unsigned long)header);

	for(unsigned i = 0; i < header->Length - sizeof(AcpiHeader); i++)
		Console::WriteFormat("%2x", ptr[i]);
	Console::Write('\n');
	ParseCode(&ptr, header->Length - sizeof(AcpiHeader));*/
	Console::WriteFormat("%d bytes of AML code\n", header->Length - sizeof(ACPI::TableHeader));
}

void SECTION(".init.text") AML::ParseCode(unsigned char **x, unsigned int len)
{
	unsigned char *start = *x;
	unsigned int length;

	while((unsigned long)(*x) < (unsigned long)start + len)
	{
		switch(((*x)++)[0])
		{
		case 0x00: // ZeroOp
		case 0x01: // OneOp
		case 0x06: // AliasOp
			NameString(x);
			Console::WriteFormat(" = ");
			NameString(x);
			Console::Write('\n');
			break;

		case 0x08: // NameOp
		case 0x0a: // ByteOp
		case 0x0b: // WordOp
		case 0x0c: // DWordOp
		case 0x0d: // StringOp
		case 0x0e: // QWordOp
		case 0x10: // ScopeOp
			length = PackageLength(x);
			NameString(x);
			Console::WriteFormat(": ScopeOp of length %d\n", length);
			ParseCode(x, length);
			break;

		case 0x11: // BufferOp
		case 0x12: // PackageOp
		case 0x13: // VarPackageOp
		case 0x14: // MethodOp
		case 0x5b: // ExtOpPrefix
			switch(((*x)++)[0])
			{
			case 0x01: // MutexOp
			case 0x02: // EventOp
			case 0x12: // CondRefOfOp
			case 0x13: // CreateFieldOp
			case 0x1f: // LoadTableOp
			case 0x20: // LoadOp
			case 0x21: // StallOp
			case 0x22: // SleepOp
			case 0x23: // AcquireOp
			case 0x24: // SignalOp
			case 0x25: // WaitOp
			case 0x26: // ResetOp
			case 0x27: // ReleaseOp
			case 0x28: // FromBCDOp
			case 0x29: // ToBCDOp
			case 0x2a: // UnloadOp
			case 0x31: // DebugOp
			case 0x32: // FatalOp
			case 0x33: // TimerOp
			case 0x80: // OpRegionOp
			case 0x82: // DeviceOp
			case 0x83: // ProcessorOp
				length = PackageLength(x);
				NameString(x);
				Console::WriteFormat(": ProcessorOp of length %d", length);
				break;

			case 0x84: // PowerResOp
			case 0x85: // ThermalZoneOp
			case 0x87: // BankFieldOp
			case 0x88: // DataRegionOp
				break;
			}
			break;

		case 0x60: // Local0Op
		case 0x61: // Local1Op
		case 0x62: // Local2Op
		case 0x63: // Local3Op
		case 0x64: // Local4Op
		case 0x65: // Local5Op
		case 0x66: // Local6Op
		case 0x67: // Local7Op
		case 0x68: // Arg0Op
		case 0x69: // Arg1Op
		case 0x6a: // Arg2Op
		case 0x6b: // Arg3Op
		case 0x6c: // Arg4Op
		case 0x6d: // Arg5Op
		case 0x6e: // Arg6Op
		case 0x70: // StoreOp
		case 0x71: // RefOfOp
		case 0x72: // AddOp
		case 0x73: // ConcatOp
		case 0x74: // SubtractOp
		case 0x75: // IncrementOp
		case 0x76: // DecrementOp
		case 0x77: // MultiplyOp
		case 0x78: // DivideOp
		case 0x79: // ShiftLeftOp
		case 0x7a: // ShiftRightOp
		case 0x7b: // AndOp
		case 0x7c: // NandOp
		case 0x7d: // OrOp
		case 0x7e: // NorOp
		case 0x7f: // XorOp
		case 0x80: // NotOp
		case 0x81: // FindSetLeftBitOp
		case 0x82: // FindSetRightBitOp
		case 0x83: // DerefOfOp
		case 0x84: // ConcatResOp
		case 0x85: // ModOp
		case 0x86: // NotifyOp
		case 0x87: // SizeOfOp
		case 0x88: // IndexOp
		case 0x89: // MatchOp
		case 0x8a: // CreateDWordFieldOp
		case 0x8b: // CreateWordFieldOp
		case 0x8c: // CreateByteFieldOp
		case 0x8d: // CreateBitFieldOp
		case 0x8e: // ObjectTypeOp
		case 0x8f: // CreateQWordFieldOp
		case 0x90: // LandOp
		case 0x91: // LorOp
		case 0x92: // LnotOp
		case 0x93: // LequalOp
		case 0x94: // LgreaterOp
		case 0x95: // LlessOp
		case 0x96: // ToBufferOp
		case 0x97: // ToDecimalStringOp
		case 0x98: // ToHexStringOp
		case 0x99: // ToIntegerOp
		case 0x9c: // ToStringOp
		case 0x9d: // CodeObjectOp
		case 0x9e: // MidOp
		case 0x9f: // ContinueOp
		case 0xa0: // IfOp
		case 0xa1: // ElseOp
		case 0xa2: // WhileOp
		case 0xa3: // NoopOp
		case 0xa4: // ReturnOp
		case 0xa5: // BreakOp
		case 0xcc: // BreakPointOp
		case 0xff: // OnesOp
		default:
			break;
		}
	}
}

unsigned int SECTION(".init.text") AML::PackageLength(unsigned char **x)
{
	unsigned char bytes = (*x)[0] >> 6;
	unsigned int length = (*x)[0] & (bytes ? 0x0f : 0x3f);

	while(bytes > 0)
	{
		length |= ((unsigned int)((*x)[bytes])) << ((bytes << 3) - 4);
		bytes--;
	}

	*x += ((*x)[0] >> 6) + 1;

	return(length);
}

void SECTION(".init.text") AML::NameString(unsigned char **x)
{
	unsigned int i;

	if((*x)[0] == '\\')
		Console::Write((*x)++[0]);

	while((*x)[0] == '^')
		Console::Write((*x)++[0]);

	switch((*x)[0])
	{
	case 0x00:
		(*x)++;
		break;

	case 0x2e:
		Console::WriteFormat("%4s\\%4s", *x + 1, *x + 5);
		*x += 9;
		break;

	case 0x2f:
		(*x)++;
		i = (*x)[0];
		(*x)++;
		while(true)
		{
			Console::WriteFormat("%4s", *x);
			*x += 4;
			if(0 == --i)
				break;
			Console::Write('\\');
		}
		break;

	default:
		Console::WriteFormat("%4s", *x);
		*x += 4;
		break;
	}
}
#endif
