// PIT.cpp - Support for programmable interval timer (Intel 8253).

#include INC_ARCH(Port.h)
#include INC_VENDOR(PIT.h)

using namespace Kernel;

char pit_space[sizeof(PIT)];

PIT::PIT(void)
{
}

void PIT::SetPeriodic(uint8_t timer, uint16_t count)
{
	lock.Lock();
	Port::WriteU8(control, (timer << 6) | MODE_BINARY | MODE_RATE | MODE_LSB_MSB);
	Port::WriteU8(baseaddr + timer, count);
	Port::WriteU8(baseaddr + timer, count >> 8);
	lock.Unlock();
}

void PIT::SetSquareWave(uint8_t timer, uint16_t count)
{
	lock.Lock();
	Port::WriteU8(control, (timer << 6) | MODE_BINARY | MODE_SQUARE | MODE_LSB_MSB);
	Port::WriteU8(baseaddr + timer, count);
	Port::WriteU8(baseaddr + timer, count >> 8);
	lock.Unlock();
}

void PIT::SetOneShot(uint8_t timer, uint16_t count)
{
	lock.Lock();
	Port::WriteU8(control, (timer << 6) | MODE_BINARY | MODE_TERM_COUNT | MODE_LSB_MSB);
	Port::WriteU8(baseaddr + timer, count);
	Port::WriteU8(baseaddr + timer, count >> 8);
	lock.Unlock();
}

uint16_t PIT::GetCurrent(uint8_t timer)
{
	uint16_t tmp;

	lock.Lock();
	Port::WriteU8(control, (timer << 6) | MODE_BINARY | MODE_LATCHING);
	tmp = Port::ReadU8(baseaddr + timer);
	tmp += (Port::ReadU8(baseaddr + timer) << 8);
	lock.Unlock();
	return(tmp);
}
