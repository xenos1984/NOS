// Apic.cpp - Advanced Programmable Interrupt Controller.

#include INC_ARCH(Apic.h)
#include INC_ARCH(X86Pager.h)
#include <Console.h>


using namespace Kernel;

//char apic_space[0x1000] __attribute__ ((aligned(0x1000)));

SECTION(".init.text") Apic::Apic(unsigned long phys)
{
	x86pager().FreeBlocks((void*)apic_space, 1);
	x86pager().MapToLinear((void*)phys, (void*)apic_space, 1);
	base[REG_SIVR] |= (1 << 8); // enable
	base[REG_LDR] = 0xff000000;
	base[REG_DFR] = 0xffffffff;
	console().WriteMessage(Console::MSG_INFO, "APIC:", "at 0x%8x, Version 0x%2x, mapped to 0x%p", phys, GetVersion(), base);
}

unsigned char Apic::GetPhysID(void) const
{
	return((base[REG_ID] >> 24) & 0x0f);
}

void Apic::SetPhysID(unsigned char id)
{
	base[REG_ID] = id << 24;
}

unsigned char Apic::GetVersion(void) const
{
	return(base[REG_VERSION] & 0xff);
}

unsigned char Apic::GetLogID(void) const
{
	return(base[REG_LDR] >> 24);
}

void Apic::SetLogID(unsigned char id)
{
	base[REG_LDR] = id << 24;
}

void Apic::SendIPI(unsigned char target, unsigned char vector, unsigned int flags)
{
	base[REG_ICR2] = target << 24;
	base[REG_ICR] = flags | vector;
}

bool Apic::SendPending(void)
{
	return(base[REG_ICR] & INT_PENDING);
}

void Apic::SignalEOI(void)
{
	base[REG_EOI] = 0;
}

unsigned char Apic::GetTimerVector(void) const
{
	return(base[REG_LVTT] & 0xff);
}

void Apic::SetTimerVector(unsigned char vector)
{
	base[REG_LVTT] = (base[REG_LVTT] & 0xffffff00) | vector;
}

unsigned char Apic::GetTimerDiv(void) const
{
	return(base[REG_TDCR] & 0x0b);
}

void Apic::SetTimerDiv(unsigned char div)
{
	base[REG_TDCR] = div;
}

unsigned int Apic::GetTimerStart(void) const
{
	return(base[REG_TIC]);
}

unsigned int Apic::GetTimerCount(void) const
{
	return(base[REG_TCC]);
}

void Apic::TimerStart(unsigned int s, bool interrupt)
{
	base[REG_LVTT] |= LVT_TPER;
	if(interrupt)
		base[REG_LVTT] &= ~LVT_MASKED;
	else
		base[REG_LVTT] |= LVT_MASKED;
	base[REG_TIC] = s;
}

void Apic::TimerStop(void)
{
	base[REG_LVTT] &= ~LVT_TPER;
	base[REG_LVTT] |= LVT_MASKED;
}

void Apic::TimerShot(unsigned int s, bool interrupt)
{
	base[REG_LVTT] &= ~LVT_TPER;
	if(interrupt)
		base[REG_LVTT] &= ~LVT_MASKED;
	else
		base[REG_LVTT] |= LVT_MASKED;
	base[REG_TIC] = s;
}

bool Apic::TimerPending(void) const
{
	return(base[REG_LVTT] & INT_PENDING);
}

unsigned char Apic::GetErrorVector(void) const
{
	return(base[REG_LVTERR] & 0xff);
}

void Apic::SetErrorVector(unsigned char vector)
{
	base[REG_LVTERR] = (base[REG_LVTERR] & 0xffffff00) | vector;
}

void Apic::MaskError(void)
{
	base[REG_LVTERR] |= LVT_MASKED;
}

void Apic::UnmaskError(void)
{
	base[REG_LVTERR] &= ~LVT_MASKED;
}

bool Apic::ErrorPending(void) const
{
	return(base[REG_LVTERR] & INT_PENDING);
}

unsigned int Apic::GetErrorStatus(void) const
{
	return(base[REG_ESR]);
}

void Apic::ClearError(void)
{
	base[REG_ESR] = 0;
}

void Apic::Enable(void)
{
	base[REG_SIVR] |= 0x00000100;
}

void Apic::Disable(void)
{
	base[REG_SIVR] &= 0xfffffeff;
}
