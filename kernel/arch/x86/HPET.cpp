// HPET.h - High Precision Event Timer

#include INC_ARCH(HPET.h)
#include <Console.h>
#include <Pager.h>
#include <Heap.h>

using namespace Kernel;

SECTION(".init.text") HPET::HPET(unsigned long phys)
{
	base = (_HPET*)Heap::Alloc(Memory::PGS_4K, Memory::PGS_4K, false);
	Pager::MapPage<Memory::PGB_4K>(phys, (uintptr_t)base, Memory::MemType::KERNEL_RW);
	Console::WriteMessage(Console::Style::INFO, "HPET:", "at 0x%8x, Rev. 0x%2x, %d counters of %d bits, %d fs", phys, GetRevision(), GetTimerCount() + 1, (Timer64Bit() ? 64 : 32), GetPeriod());
}

unsigned char HPET::GetRevision(void) const
{
	return(base->RevID);
}

unsigned char HPET::GetTimerCount(void) const
{
	return(base->TimerCount);
}

bool HPET::Timer64Bit(void) const
{
	return(base->Timer64Bit);
}

bool HPET::LegacyCapable(void) const
{
	return(base->LegRouteCap);
}

unsigned short HPET::GetVendor(void) const
{
	return(base->VendorID);
}

unsigned int HPET::GetPeriod(void) const
{
	return(base->ClockPeriod);
}

bool HPET::InterruptActive(int n) const
{
	return((base->InterruptStatus >> n) & 1);
}

void HPET::ClearInterrupt(int n)
{
	base->InterruptStatus |= (1 << n);
}

uint64_t HPET::GetCurrentCount64(void) const
{
	return(base->MainCounter.Value64);
}

void HPET::SetCurrentCount64(uint64_t c)
{
	base->MainCounter.Value64 = c;
}

uint32_t HPET::GetCurrentCount32(void) const
{
	return(base->MainCounter.Value32.Low);
}

void HPET::SetCurrentCount32(uint32_t c)
{
	base->MainCounter.Value32.Low = c;
}

uint64_t HPET::GetComparator64(int n) const
{
	return(base->Timer[n].Comparator.Value64);
}

void HPET::SetComparator64(int n, uint64_t c)
{
	base->Timer[n].Comparator.Value64 = c;
}

uint32_t HPET::GetComparator32(int n) const
{
	return(base->Timer[n].Comparator.Value32.Low);
}

void HPET::SetComparator32(int n, uint32_t c)
{
	base->Timer[n].Comparator.Value32.Low = c;
}
