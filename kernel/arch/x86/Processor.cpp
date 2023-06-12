// Processor.cpp - Processor management.

#include <Console.h>
#include <Symbol.h>
#include <Processor.h>
#include INC_VENDOR(Cmos.h)
#include INC_VENDOR(PIT.h)
#include INC_ARCH(Apic.h)

using namespace Kernel;
/*
extern "C" volatile unsigned long apcount;

#if defined CONFIG_SMP || defined CONFIG_ACPI
bool SECTION(".init.text") Processor::Startup(void)
{
	unsigned long count = apcount;

	Cmos::SetShutdownStatus(Cmos::SH_JMP467);
	*(unsigned int *)(0x00000467 + Symbol::kernelOffset.Addr()) = (AP_INIT_ADDR >> 12);
	Apic::ClearError();
	Apic::SendIPI(data.physID, 0, Apic::DEST_FIELD | Apic::INT_ASSERT | Apic::DM_INIT); // assert INIT IPI
	while(Apic::SendPending()) ;
	Apic::ClearError();
	Apic::SendIPI(data.physID, 0, Apic::DEST_FIELD | Apic::DM_INIT); // deassert INIT IPI
	while(Apic::SendPending()) ;
	PIT::SetOneShot(0, 11932); // 10ms
	while(PIT::GetCurrent(0) <= 11932) ;

	if(Apic::GetVersion() >= 0x10)
	{
		Apic::ClearError();
		Apic::SendIPI(data.physID, AP_INIT_ADDR >> 12, Apic::DEST_FIELD | Apic::DM_STARTUP);
		while(Apic::SendPending()) ;
		PIT::SetOneShot(0, 239); // 200µs
		while(PIT::GetCurrent(0) <= 239) ;
		if(count == apcount)
		{
			Apic::ClearError();
			Apic::SendIPI(data.physID, AP_INIT_ADDR >> 12, Apic::DEST_FIELD | Apic::DM_STARTUP);
			while(Apic::SendPending()) ;
			PIT::SetOneShot(0, 239); // 200µs
			while(PIT::GetCurrent(0) <= 239) ;
		}
	}

	PIT::SetOneShot(0, 11932); // 10ms
	while((PIT::GetCurrent(0) <= 11932) && (count == apcount)) ;
	Cmos::SetShutdownStatus(Cmos::SH_NORMAL);

	if(apcount > count)
	{
		Console::WriteMessage(Console::Style::OK, "AP no. 0x%2x: ", "booted", data.physID);
		return true;
	}
	else
	{
		Console::WriteMessage(Console::Style::WARNING, "AP no. 0x%2x: ", "no response, disabled", data.physID);
		return false;
	}
}
#endif */
