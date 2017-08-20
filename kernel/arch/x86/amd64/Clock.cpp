// Clock.cpp - System clock.

#include <Clock.h>
#include <Console.h>
#include INC_VENDOR(Cmos.h)
#include INC_ARCH(Apic.h)
#include INC_SUBARCH(PMode.h)

using namespace Kernel;

char clock_space[sizeof(Clock)];

SECTION(".init.text") Clock::Clock(void)
{
	DateTime dt;
	static const unsigned int dateoffs[13] = {0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

	cmos().WaitForSecond();
	startticks = ReadTimeStamp();

	if(cmos().GetDateTime(dt))
	{
		Console::WriteMessage(Console::Style::OK, "CMOS time:", "%4d/%2d/%2d, %2d:%2d:%2d", dt.Year, dt.Month, dt.Day, dt.Hour, dt.Minute, dt.Second);
		starttime = (((dt.Year - 1970) * 365 + (dt.Year - (dt.Month > 2 ? 1968 : 1972)) / 4 + dateoffs[dt.Month] + dt.Day - 1) * 86400 + dt.Hour * 3600 + dt.Minute * 60 + dt.Second);
	}
	else
	{
		Console::WriteMessage(Console::Style::WARNING, "CMOS time:", "invalid");
		starttime = (((30 * 365) + 7) * 86400) << 24;
	}

	Console::WriteMessage(Console::Style::INFO, "Timestamp:", "%lu", starttime);
	starttime = starttime << 24;

	cmos().WaitForSecond();
	speed = ReadTimeStamp() - startticks;
	Console::WriteMessage(Console::Style::INFO, "CPU speed:", "%lu Hz", speed);

	if(speed < (1 << 24))
		Apic::SetTimerDiv(Apic::TDR_1);
	else if(speed < (1 << 25))
		Apic::SetTimerDiv(Apic::TDR_2);
	else if(speed < (1 << 26))
		Apic::SetTimerDiv(Apic::TDR_4);
	else if(speed < (1 << 27))
		Apic::SetTimerDiv(Apic::TDR_8);
	else if(speed < (1 << 28))
		Apic::SetTimerDiv(Apic::TDR_16);
	else if(speed < (1 << 29))
		Apic::SetTimerDiv(Apic::TDR_32);
	else if(speed < (1 << 30))
		Apic::SetTimerDiv(Apic::TDR_64);
	else
		Apic::SetTimerDiv(Apic::TDR_128);

	Apic::SetTimerVector(0x40);
	Apic::TimerStart(0x10000);
}
