// Processor.cpp - Processor management.

#include <cstddef>
#include <Console.h>
#include <Symbol.h>
#include INC_VENDOR(Cmos.h)
#include INC_VENDOR(PIT.h)
#include INC_VENDOR(ACPI.h)
#include INC_VENDOR(SMP.h)
#include INC_ARCH(Apic.h)
#include INC_ARCH(Processor.h)
#include INC_ARCH(CPU.h)

using namespace Kernel;

extern bool apflag;
extern "C" unsigned long apstack;
extern "C" unsigned char apentry[];

Processor* Processor::bsp = nullptr;
Processor* Processor::procs = nullptr;
unsigned int Processor::count = 0;

SECTION(".init.text") Processor::Processor(void)
{
	physID = bspcpu().GetPhysID();
}

#ifdef CONFIG_ACPI
SECTION(".init.text") Processor::Processor(ACPI::LApic* ala)
{
	physID = ala->ApicID;
}
#endif

#ifdef CONFIG_SMP
SECTION(".init.text") Processor::Processor(SMP::Cpu* sc)
{
	physID = sc->LocalApicID;
}
#endif

Processor::~Processor(void)
{
}

#if defined CONFIG_SMP || defined CONFIG_ACPI
void SECTION(".init.text") Processor::Startup(unsigned long stack)
{
	long i;

	apstack = stack;

	for(i = 0; i < 4096; i++)
		((unsigned char*)(AP_INIT_ADDR + Symbol::kernelOffset.Addr()))[i] = apentry[i];

	apflag = false;
	Cmos::SetShutdownStatus(Cmos::SH_JMP467);
	*(unsigned int *)(0x00000467 + Symbol::kernelOffset.Addr()) = (AP_INIT_ADDR >> 12);
	Apic::ClearError();
	Apic::SendIPI(physID, 0, Apic::DEST_FIELD | Apic::INT_ASSERT | Apic::DM_INIT); // assert INIT IPI
	while(Apic::SendPending()) ;
	Apic::ClearError();
	Apic::SendIPI(physID, 0, Apic::DEST_FIELD | Apic::DM_INIT); // deassert INIT IPI
	while(Apic::SendPending()) ;
	pit().SetOneShot(0, 11932); // 10ms
	while(pit().GetCurrent(0) <= 11932) ;
	if(Apic::GetVersion() >= 0x10)
	{
		Apic::ClearError();
		Apic::SendIPI(physID, AP_INIT_ADDR >> 12, Apic::DEST_FIELD | Apic::DM_STARTUP);
		while(Apic::SendPending()) ;
		pit().SetOneShot(0, 239); // 200µs
		while(pit().GetCurrent(0) <= 239) ;
		if(!apflag)
		{
			Apic::ClearError();
			Apic::SendIPI(physID, AP_INIT_ADDR >> 12, Apic::DEST_FIELD | Apic::DM_STARTUP);
			while(Apic::SendPending()) ;
			pit().SetOneShot(0, 239); // 200µs
			while(pit().GetCurrent(0) <= 239) ;
		}
	}
	pit().SetOneShot(0, 11932); // 10ms
	while((pit().GetCurrent(0) <= 11932) && !apflag) ;
	Cmos::SetShutdownStatus(Cmos::SH_NORMAL);
	if(apflag)
		Console::WriteMessage(Console::Style::OK, "AP no. 0x%2x: ", "booted", physID);
	else
		Console::WriteMessage(Console::Style::WARNING, "AP no. 0x%2x: ", "no response, disabled", physID);
}

void Processor::Shutdown(void)
{
}
#endif
