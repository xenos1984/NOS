// Apic.cpp - Advanced Programmable Interrupt Controller.

#include INC_ARCH(Apic.h)
#include <Memory.h>
#include <Chunker.h>
#include <Pager.h>
#include <Console.h>

namespace Kernel
{
	namespace Apic
	{
		SECTION(".init.text") void Init(unsigned long phys)
		{
			Chunker::Free(Pager::VirtToPhys((uintptr_t)apic_base));
			Pager::UnmapPage<Memory::PGB_4K>((uintptr_t)apic_base);
			Pager::MapPage<Memory::PGB_4K>(phys, (uintptr_t)apic_base, Memory::MemType::KERNEL_RW);
			apic_base[REG_SIVR] |= (1 << 8); // enable
			apic_base[REG_LDR] = 0xff000000;
			apic_base[REG_DFR] = 0xffffffff;
			Console::WriteMessage(Console::Style::INFO, "APIC:", "at 0x%8x, Version 0x%2x, mapped to 0x%p", phys, GetVersion(), apic_base);
		}

		unsigned char GetPhysID(void)
		{
			return((apic_base[REG_ID] >> 24) & 0x0f);
		}

		void SetPhysID(unsigned char id)
		{
			apic_base[REG_ID] = id << 24;
		}

		unsigned char GetVersion(void)
		{
			return(apic_base[REG_VERSION] & 0xff);
		}

		unsigned char GetLogID(void)
		{
			return(apic_base[REG_LDR] >> 24);
		}

		void SetLogID(unsigned char id)
		{
			apic_base[REG_LDR] = id << 24;
		}

		void SendIPI(unsigned char target, unsigned char vector, unsigned int flags)
		{
			apic_base[REG_ICR2] = target << 24;
			apic_base[REG_ICR] = flags | vector;
		}

		bool SendPending(void)
		{
			return(apic_base[REG_ICR] & INT_PENDING);
		}

		void SignalEOI(void)
		{
			apic_base[REG_EOI] = 0;
		}

		unsigned char GetTimerVector(void)
		{
			return(apic_base[REG_LVTT] & 0xff);
		}

		void SetTimerVector(unsigned char vector)
		{
			apic_base[REG_LVTT] = (apic_base[REG_LVTT] & 0xffffff00) | vector;
		}

		unsigned char GetTimerDiv(void)
		{
			return(apic_base[REG_TDCR] & 0x0b);
		}

		void SetTimerDiv(unsigned char div)
		{
			apic_base[REG_TDCR] = div;
		}

		unsigned int GetTimerStart(void)
		{
			return(apic_base[REG_TIC]);
		}

		unsigned int GetTimerCount(void)
		{
			return(apic_base[REG_TCC]);
		}

		void TimerStart(unsigned int s, bool interrupt)
		{
			apic_base[REG_LVTT] |= LVT_TPER;
			if(interrupt)
				apic_base[REG_LVTT] &= ~LVT_MASKED;
			else
				apic_base[REG_LVTT] |= LVT_MASKED;
			apic_base[REG_TIC] = s;
		}

		void TimerStop(void)
		{
			apic_base[REG_LVTT] &= ~LVT_TPER;
			apic_base[REG_LVTT] |= LVT_MASKED;
		}

		void TimerShot(unsigned int s, bool interrupt)
		{
			apic_base[REG_LVTT] &= ~LVT_TPER;
			if(interrupt)
				apic_base[REG_LVTT] &= ~LVT_MASKED;
			else
				apic_base[REG_LVTT] |= LVT_MASKED;
			apic_base[REG_TIC] = s;
		}

		bool TimerPending(void)
		{
			return(apic_base[REG_LVTT] & INT_PENDING);
		}

		unsigned char GetErrorVector(void)
		{
			return(apic_base[REG_LVTERR] & 0xff);
		}

		void SetErrorVector(unsigned char vector)
		{
			apic_base[REG_LVTERR] = (apic_base[REG_LVTERR] & 0xffffff00) | vector;
		}

		void MaskError(void)
		{
			apic_base[REG_LVTERR] |= LVT_MASKED;
		}

		void UnmaskError(void)
		{
			apic_base[REG_LVTERR] &= ~LVT_MASKED;
		}

		bool ErrorPending(void)
		{
			return(apic_base[REG_LVTERR] & INT_PENDING);
		}

		unsigned int GetErrorStatus(void)
		{
			return(apic_base[REG_ESR]);
		}

		void ClearError(void)
		{
			apic_base[REG_ESR] = 0;
		}

		void Enable(void)
		{
			apic_base[REG_SIVR] |= 0x00000100;
		}

		void Disable(void)
		{
			apic_base[REG_SIVR] &= 0xfffffeff;
		}
	}
}
