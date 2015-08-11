// Processor.cpp - Processor management.

#include <cstddef>
#include <Console.h>
#include <Memory.h>
#include INC_ARCH(Cmos.h)
#include INC_ARCH(PIT.h)
#include INC_ARCH(ACPI.h)
#include INC_ARCH(SMP.h)
#include INC_ARCH(Apic.h)
#include INC_ARCH(Processor.h)
#include INC_ARCH(CPU.h)

using namespace Kernel;

extern bool apflag;
extern "C" unsigned long apstack;
extern "C" unsigned char apentry[];

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
		((unsigned char*)(AP_INIT_ADDR + Memory::kernelOffset.Addr()))[i] = apentry[i];

	apflag = false;
	cmos().SetShutdownStatus(Cmos::SH_JMP467);
	*(unsigned int *)(0x00000467 + Memory::kernelOffset.Addr()) = (AP_INIT_ADDR >> 12);
	apic().ClearError();
	apic().SendIPI(physID, 0, Apic::DEST_FIELD | Apic::INT_ASSERT | Apic::DM_INIT); // assert INIT IPI
	while(apic().SendPending()) ;
	apic().ClearError();
	apic().SendIPI(physID, 0, Apic::DEST_FIELD | Apic::DM_INIT); // deassert INIT IPI
	while(apic().SendPending()) ;
	pit().SetOneShot(0, 11932); // 10ms
	while(pit().GetCurrent(0) <= 11932) ;
	if(apic().GetVersion() >= 0x10)
	{
		apic().ClearError();
		apic().SendIPI(physID, AP_INIT_ADDR >> 12, Apic::DEST_FIELD | Apic::DM_STARTUP);
		while(apic().SendPending()) ;
		pit().SetOneShot(0, 239); // 200µs
		while(pit().GetCurrent(0) <= 239) ;
		if(!apflag)
		{
			apic().ClearError();
			apic().SendIPI(physID, AP_INIT_ADDR >> 12, Apic::DEST_FIELD | Apic::DM_STARTUP);
			while(apic().SendPending()) ;
			pit().SetOneShot(0, 239); // 200µs
			while(pit().GetCurrent(0) <= 239) ;
		}
	}
	pit().SetOneShot(0, 11932); // 10ms
	while((pit().GetCurrent(0) <= 11932) && !apflag) ;
	cmos().SetShutdownStatus(Cmos::SH_NORMAL);
	if(apflag)
		console().WriteMessage(Console::MSG_OK, "AP no. 0x%2x: ", "booted", physID);
	else
		console().WriteMessage(Console::MSG_WARNING, "AP no. 0x%2x: ", "no response, disabled", physID);
}

void Processor::Shutdown(void)
{
}
#endif
