// Cmos.cpp - Support for MC146818 CMOS RAM / Real Time Clock.

#include <AtomicOps.h>
#include <Console.h>
#include INC_ARCH(Port.h)
#include INC_VENDOR(Cmos.h)

using namespace Kernel;

char cmos_space[sizeof(Cmos)];

Cmos::Cmos(void)
{
	uint8_t sec, min, hour, day, mon, year, cent, mode;

	lock.Enter();

	Port::WriteU8(CMOS_ADDR, REG_STATUS_D);
	if((Port::ReadU8(CMOS_DATA) & D_VALID) == 0)
	{
		lock.Exit();
		Console::WriteMessage(Console::Style::WARNING, "CMOS time:", "Invalid");
		return;
	}

	Port::WriteU8(CMOS_ADDR, REG_STATUS_A);
	while(Port::ReadU8(CMOS_DATA) & A_UPDATE) ;

	Port::WriteU8(CMOS_ADDR, REG_SEC);
	sec = Port::ReadU8(CMOS_DATA);

	Port::WriteU8(CMOS_ADDR, REG_MIN);
	min = Port::ReadU8(CMOS_DATA);

	Port::WriteU8(CMOS_ADDR, REG_HOUR);
	hour = Port::ReadU8(CMOS_DATA);

	Port::WriteU8(CMOS_ADDR, REG_DAY);
	day = Port::ReadU8(CMOS_DATA);

	Port::WriteU8(CMOS_ADDR, REG_MONTH);
	mon = Port::ReadU8(CMOS_DATA);

	Port::WriteU8(CMOS_ADDR, REG_YEAR);
	year = Port::ReadU8(CMOS_DATA);

	Port::WriteU8(CMOS_ADDR, REG_CENT);
	cent = Port::ReadU8(CMOS_DATA);

	Port::WriteU8(CMOS_ADDR, REG_STATUS_B);
	mode = Port::ReadU8(CMOS_DATA);

	lock.Exit();

	if(mode & B_BCD)
		Console::WriteMessage(Console::Style::OK, "CMOS time (Dec):", "%2d%2d/%2d/%2d, %2d:%2d:%2d", cent, year, mon, day, hour, min, sec);
	else
		Console::WriteMessage(Console::Style::OK, "CMOS time (BCD):", "%2x%2x/%2x/%2x, %2x:%2x:%2x", cent, year, mon, day, hour, min, sec);
}

uint8_t Cmos::ReadRegister(uint8_t reg)
{
	uint8_t tmp;

	lock.Enter();
	Port::WriteU8(CMOS_ADDR, reg);
	tmp = Port::ReadU8(CMOS_DATA);
	lock.Exit();
	return(tmp);
}

void Cmos::WriteRegister(uint8_t reg, uint8_t value)
{
	lock.Enter();
	Port::WriteU8(CMOS_ADDR, reg);
	Port::WriteU8(CMOS_DATA, value);
	lock.Exit();
}

Time Cmos::GetTime(void)
{
	long sec, min, hour, day, mon, year, cent;
	uint8_t mode;
	static const unsigned long dateoffs[13] = {0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

	lock.Enter();

	Port::WriteU8(CMOS_ADDR, REG_STATUS_D);
	if((Port::ReadU8(CMOS_DATA) & D_VALID) == 0)
	{
		lock.Exit();
		return Time(0);
	}

	Port::WriteU8(CMOS_ADDR, REG_STATUS_A);
	while(Port::ReadU8(CMOS_DATA) & A_UPDATE) ;

	Port::WriteU8(CMOS_ADDR, REG_SEC);
	sec = Port::ReadU8(CMOS_DATA);

	Port::WriteU8(CMOS_ADDR, REG_MIN);
	min = Port::ReadU8(CMOS_DATA);

	Port::WriteU8(CMOS_ADDR, REG_HOUR);
	hour = Port::ReadU8(CMOS_DATA);

	Port::WriteU8(CMOS_ADDR, REG_DAY);
	day = Port::ReadU8(CMOS_DATA);

	Port::WriteU8(CMOS_ADDR, REG_MONTH);
	mon = Port::ReadU8(CMOS_DATA);

	Port::WriteU8(CMOS_ADDR, REG_YEAR);
	year = Port::ReadU8(CMOS_DATA);

	Port::WriteU8(CMOS_ADDR, REG_CENT);
	cent = Port::ReadU8(CMOS_DATA);

	Port::WriteU8(CMOS_ADDR, REG_STATUS_B);
	mode = Port::ReadU8(CMOS_DATA);

	lock.Exit();

	if((mode & B_BCD) == 0)
	{
		sec = (sec & 0x0f) + 10 * (sec >> 4);
		min = (min & 0x0f) + 10 * (min >> 4);
		hour = (hour & 0x0f) + 10 * (hour >> 4);
		day = (day & 0x0f) + 10 * (day >> 4);
		mon = (mon & 0x0f) + 10 * (mon >> 4);
		year = (year & 0x0f) + 10 * (year >> 4);
		cent = (cent & 0x0f) + 10 * (cent >> 4);
	}

	return Time(((100 * cent + year - 1970) * 365 + (100 * cent + year - (mon > 2 ? 1968 : 1972)) / 4 + dateoffs[mon] + day - 1) * 86400 + hour * 3600 + min * 60 + sec, 0);
}

void Cmos::WaitForSecond(void)
{
	uint8_t sec;

	lock.Enter();
	Port::WriteU8(CMOS_ADDR, REG_STATUS_A);
	while(Port::ReadU8(CMOS_DATA) & A_UPDATE) ;
	Port::WriteU8(CMOS_ADDR, REG_SEC);
	sec = Port::ReadU8(CMOS_DATA);
	while(sec == Port::ReadU8(CMOS_DATA));
	lock.Exit();
}

void Cmos::SetPeriodic(uint8_t div)
{
	lock.Enter();
	Port::WriteU8(CMOS_ADDR, REG_STATUS_A);
	while(Port::ReadU8(CMOS_DATA) & A_UPDATE) ; // wait for update-in-progress
	Port::WriteU8(CMOS_DATA, (Port::ReadU8(CMOS_DATA) & 0xf0) | (div & 0x0f)); // set rate
	Port::WriteU8(CMOS_ADDR, REG_STATUS_B);
	Port::WriteU8(CMOS_DATA, Port::ReadU8(CMOS_DATA) | IRQ_PER); // enable periodic interrupt
	Port::WriteU8(CMOS_ADDR, REG_STATUS_C);
	Port::ReadU8(CMOS_DATA); // clear interrupt pending
	lock.Exit();
}
