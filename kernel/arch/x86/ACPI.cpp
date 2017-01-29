// ACPI.cpp - Advanced Configuration and Power Interface.

#include <new>
#include INC_ARCH(ACPI.h)
#include INC_ARCH(AML.h)
#include INC_ARCH(X86Pager.h)
#include INC_ARCH(Apic.h)
#include INC_ARCH(IOApic.h)
#include INC_ARCH(HPET.h)
#include <Console.h>
#include <Symbol.h>

using namespace Kernel;

#ifdef CONFIG_ACPI
char acpi_space[sizeof(ACPI)];

SECTION(".init.text") ACPI::ACPI(TableHeader* header, unsigned int rev)
{
	RsdTable* rsdt;
	XsdTable* xsdt;
	unsigned long i;

	Console::WriteMessage(Console::Style::INFO, "ACPI %4s:", "OEMID: %6s, OEM Table ID: %8s, OEM Rev.: %d", (header->Signature).Bytes, header->OEMID, header->OEMTabID, header->OEMRev);
	Console::WriteMessage(Console::Style::INFO, "ACPI %4s:", "Creator ID: %4s, Creator Rev.: %d", (header->Signature).Bytes, header->CreatorID, header->CreatorRev);

	numcpus = numioapics = numirqsrcs = numnmisrcs = numlapicnmis = numlapicaddrs = numiosapics = numlsapics = numplatirqs = numx2apics = numx2apicnmis = 0;

	if(rev == 0)
	{
		rsdt = (RsdTable*)header;
		for(i = 0; 4 * i + sizeof(TableHeader) < (rsdt->Header).Length; i++)
			ParseTable(rsdt->Entry[i]);
	}
	else
	{
		xsdt = (XsdTable*)header;
		for(i = 0; 8 * i + sizeof(TableHeader) < (xsdt->Header).Length; i++)
			ParseTable(xsdt->Entry[i]);
	}
}

void SECTION(".init.text") ACPI::ParseFacs(Facs* facs)
{
	Console::WriteMessage(Console::Style::INFO, "Hardware Signature:", "0x%8x", facs->HardwareSignature);
}

void SECTION(".init.text") ACPI::ParseMadt(MadTable* madt)
{
	unsigned char* ptr;

	Console::WriteMessage(Console::Style::INFO, "CPU mode:", "ACPI detected");
	Console::WriteMessage(Console::Style::INFO, "ACPI MADT:", "OEMID: %6s, OEM Table ID: %8s, OEM Rev.: %d", (madt->Header).OEMID, (madt->Header).OEMTabID, (madt->Header).OEMRev);
	Apic::Init(madt->ApicAddress);

	for(ptr = (unsigned char*)((unsigned long)madt + sizeof(MadTable)); (unsigned long)ptr < (unsigned long)madt + (madt->Header).Length; ptr += ptr[1])
	{
		switch(ptr[0])
		{
		case ENTRY_LAPIC:
			numcpus++;
			break;
		case ENTRY_IOAPIC:
			numioapics++;
			break;
		case ENTRY_IRQ_SRC:
			numirqsrcs++;
			break;
		case ENTRY_NMI:
			numnmisrcs++;
			break;
		case ENTRY_LAPIC_NMI:
			numlapicnmis++;
			break;
		case ENTRY_LAPIC_ADDR:
			numlapicaddrs++;
			break;
		case ENTRY_IOSAPIC:
			numiosapics++;
			break;
		case ENTRY_LSAPIC:
			numlsapics++;
			break;
		case ENTRY_PLATFORM:
			numplatirqs++;
			break;
		case ENTRY_X2APIC:
			numx2apics++;
			break;
		case ENTRY_X2APIC_NMI:
			numx2apicnmis++;
			break;
		default:
			break;
		}
	}

	cpus = new LApic*[numcpus];
	ioapics = new IOApic*[numioapics];
	irqsrcs = new IrqSourceOverride*[numirqsrcs];
	nmisrcs = new NMI*[numnmisrcs];
	lapicnmis = new LApicNMI*[numlapicnmis];
	lapicaddrs = new LApicAddressOverride*[numlapicaddrs];
	iosapics = new IOSApic*[numiosapics];
	lsapics = new LSApic*[numlsapics];
	platirqs = new PlatformInterrupt*[numplatirqs];
	x2apics = new X2Apic*[numx2apics];
	x2apicnmis = new X2ApicNMI*[numx2apicnmis];

	numcpus = numioapics = numirqsrcs = numnmisrcs = numlapicnmis = numlapicaddrs = numiosapics = numlsapics = numplatirqs = numx2apics = numx2apicnmis = 0;

	for(ptr = (unsigned char*)((unsigned long)madt + sizeof(MadTable)); (unsigned long)ptr < (unsigned long)madt + (madt->Header).Length; ptr += ptr[1])
	{
		switch(ptr[0])
		{
		case ENTRY_LAPIC:
			cpus[numcpus] = (LApic*)ptr;
			Console::WriteMessage(Console::Style::INFO, "CPU:", "CPU %d, LAPIC ID %d", cpus[numcpus]->ProcessorID, cpus[numcpus]->ApicID);
			numcpus++;
			break;
		case ENTRY_IOAPIC:
			ioapics[numioapics] = (IOApic*)ptr;
			Console::WriteMessage(Console::Style::INFO, "I/O APIC:", "#%d (IRQ base %d) @ 0x%8x", ioapics[numioapics]->ApicID, ioapics[numioapics]->InterruptBase, ioapics[numioapics]->Address);
			numioapics++;
			break;
		case ENTRY_IRQ_SRC:
			irqsrcs[numirqsrcs] = (IrqSourceOverride*)ptr;
			Console::WriteMessage(Console::Style::INFO, "IRQ SRC:", "Bus #%d IRQ #%d => INT #%d", irqsrcs[numirqsrcs]->Bus, irqsrcs[numirqsrcs]->Source, irqsrcs[numirqsrcs]->Interrupt);
			numirqsrcs++;
			break;
		case ENTRY_NMI:
			nmisrcs[numnmisrcs] = (NMI*)ptr;
			Console::WriteMessage(Console::Style::INFO, "NMI:", "INT #%d", nmisrcs[numnmisrcs]->Interrupt);
			numnmisrcs++;
			break;
		case ENTRY_LAPIC_NMI:
			lapicnmis[numlapicnmis] = (LApicNMI*)ptr;
			Console::WriteMessage(Console::Style::INFO, "LAPIC NMI:", "CPU #%d, LINT #%d", lapicnmis[numlapicnmis]->ProcessorID, lapicnmis[numlapicnmis]->LINT);
			numlapicnmis++;
			break;
		case ENTRY_LAPIC_ADDR:
			lapicaddrs[numlapicaddrs] = (LApicAddressOverride*)ptr;
			Console::WriteMessage(Console::Style::INFO, "LAPIC Addr:", "Override to 0x%16lx", lapicaddrs[numlapicaddrs]->Address);
			numlapicaddrs++;
			break;
		case ENTRY_IOSAPIC:
			iosapics[numiosapics] = (IOSApic*)ptr;
			Console::WriteMessage(Console::Style::INFO, "I/O SAPIC:", "#%d (IRQ base %d) @ 0x%16lx", iosapics[numiosapics]->ApicID, iosapics[numiosapics]->InterruptBase, iosapics[numiosapics]->Address);
			numiosapics++;
			break;
		case ENTRY_LSAPIC:
			lsapics[numlsapics] = (LSApic*)ptr;
			Console::WriteMessage(Console::Style::INFO, "LSAPIC:", "CPU #%d, LSAPIC ID %d, LSAPIC EID %d", lsapics[numlsapics]->ProcessorID, lsapics[numlsapics]->LSApicID, lsapics[numlsapics]->LSApicEID);
			numlsapics++;
			break;
		case ENTRY_PLATFORM:
			platirqs[numplatirqs] = (PlatformInterrupt*)ptr;
			Console::WriteMessage(Console::Style::INFO, "PMI:", "Dest ID %d, Dest EID %d, Vector %d, INT #%d", platirqs[numplatirqs]->ProcessorID, platirqs[numplatirqs]->ProcessorEID, platirqs[numplatirqs]->Vector, platirqs[numplatirqs]->Interrupt);
			numplatirqs++;
			break;
		case ENTRY_X2APIC:
			x2apics[numx2apics] = (X2Apic*)ptr;
			Console::WriteMessage(Console::Style::INFO, "X2APIC:", "X2APIC ID %d, CPU %4s", x2apics[numx2apics]->X2ApicID, &(x2apics[numx2apics]->AcpiID));
			numx2apics++;
			break;
		case ENTRY_X2APIC_NMI:
			x2apicnmis[numx2apicnmis] = (X2ApicNMI*)ptr;
			Console::WriteMessage(Console::Style::INFO, "X2APIC NMI:", "CPU %4s, LINT #%d", &(x2apicnmis[numx2apicnmis]->AcpiID), x2apicnmis[numx2apicnmis]->LINT);
			numx2apicnmis++;
			break;
		default:
			break;
		}
	}
}

void SECTION(".init.text") ACPI::ParseFadt(FadTable* fadt)
{
	ParseFacs((Facs*)x86pager().PhysToVirt(fadt->FacsAddress));
	AML::ParseTable((TableHeader*)x86pager().PhysToVirt(fadt->DsdtAddress));
}

void SECTION(".init.text") ACPI::ParseHpet(HpetTable* hpet)
{
	Console::WriteMessage(Console::Style::INFO, "HPET # %d:", "%d %d-bit counters found at 0x%8x", hpet->Number, (hpet->EventTimerBlockID).Data.ComparatorCount + 1, ((hpet->EventTimerBlockID).Data.CountSize64Bit ? 64 : 32), (hpet->BaseAddress).Address);
	new HPET((hpet->BaseAddress).Address);
}

void SECTION(".init.text") ACPI::ParseTable(unsigned long phys)
{
	TableHeader* header = (TableHeader*)x86pager().PhysToVirt(phys);
	unsigned char checksum;
	unsigned char* csptr;
	unsigned long i;

	checksum = 0;
	csptr = (unsigned char*)header;
	for(i = 0; i < header->Length; i++)
		checksum += csptr[i];
	if(checksum > 0)
	{
		Console::WriteMessage(Console::Style::WARNING, "ACPI %4s:", "checksum failed at 0x%8x, ignored", (header->Signature).Bytes, phys);
		return;
	}

	Console::WriteMessage(Console::Style::OK, "ACPI %4s:", "found at 0x%8x", (header->Signature).Bytes, phys);

	switch((header->Signature).Number)
	{
	case 'D' | ('S' << 8) | ('D' << 16) | ('T' << 24):
	case 'S' | ('S' << 8) | ('D' << 16) | ('T' << 24):
		AML::ParseTable(header);
		break;
	case 'F' | ('A' << 8) | ('C' << 16) | ('P' << 24):
		ParseFadt((FadTable*)header);
		break;
	case 'A' | ('P' << 8) | ('I' << 16) | ('C' << 24):
		ParseMadt((MadTable*)header);
		break;
	case 'H' | ('P' << 8) | ('E' << 16) | ('T' << 24):
		ParseHpet((HpetTable*)header);
		break;
	default:
		break;
	}
}

bool SECTION(".init.text") ACPI::Init(unsigned long first, unsigned long last)
{
	RsdPtr* pointer;
	TableHeader* header;
	unsigned char checksum;
	unsigned char* csptr;
	unsigned long phys, tab;
	unsigned int i;

	for(phys = first; phys < last; phys += 16) // 16 byte aligned
	{
		pointer = (RsdPtr*)(phys + Symbol::kernelOffset.Addr());
		if((pointer->Signature[0] == 'R') && (pointer->Signature[1] == 'S') && (pointer->Signature[2] == 'D') && (pointer->Signature[3] == ' ') &&
			(pointer->Signature[4] == 'P') && (pointer->Signature[5] == 'T') && (pointer->Signature[6] == 'R') && (pointer->Signature[7] == ' '))
		{
			checksum = 0;
			csptr = (unsigned char*)pointer;
			for(i = 0; i < 20; i++)
				checksum += csptr[i];
			if(checksum > 0)
			{
				Console::WriteMessage(Console::Style::WARNING, "ACPI RSD pointer:", "1st checksum failed at 0x%8x, ignored", phys);
				continue;
			}
			if(pointer->Revision > 0)
			{
				checksum = 0;
				csptr = (unsigned char*)pointer;
				for(i = 0; i < 36; i++)
					checksum += csptr[i];
				if(checksum > 0)
				{
					Console::WriteMessage(Console::Style::WARNING, "ACPI RSD pointer:", "2nd checksum failed at 0x%8x, ignored", phys);
					continue;
				}
			}
			Console::WriteMessage(Console::Style::OK, "ACPI RSD pointer:", "Rev. %d found at 0x%8x", pointer->Revision, phys);
			tab = ((pointer->Revision == 0) ? pointer->RsdtAddress : pointer->XsdtAddress);
			header = (TableHeader*)x86pager().PhysToVirt(tab);
			Console::WriteMessage(Console::Style::OK, "ACPI %4s:", "found at 0x%8x", (header->Signature).Bytes, tab);
			checksum = 0;
			csptr = (unsigned char*)header;
			for(i = 0; i < header->Length; i++)
				checksum += csptr[i];
			if(checksum > 0)
			{
				Console::WriteMessage(Console::Style::WARNING, "ACPI %4s:", "checksum failed, ignored", (header->Signature).Bytes);
				continue;
			}
			new (acpi_space) ACPI(header, pointer->Revision);
			return(true);
		}
	}
	Console::WriteMessage(Console::Style::INFO, "ACPI RSD pointer:", "not found in range 0x%8x to 0x%8x", first, last);
	return(false);
}

int SECTION(".init.text") ACPI::GetISAIRQ(int n)
{
	int i;
	for(i = 0; i < numirqsrcs; i++)
	{
		if(irqsrcs[i]->Source == n)
			return(irqsrcs[i]->Interrupt);
	}
	return(n);
}
#endif
