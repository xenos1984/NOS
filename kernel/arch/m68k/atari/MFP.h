// MFP.h - Atari multi-function peripheral MC68901.

#ifndef __ARCH_M68K_ATARI_MFP_H__
#define __ARCH_M68K_ATARI_MFP_H__

#include <cstdint>

namespace Kernel
{
	class MFP
	{
	private:
		struct _mfp
		{
			volatile uint8_t space1;
			volatile uint8_t GpipData;
			volatile uint8_t space2;
			volatile uint8_t ActiveEdge;
			volatile uint8_t space3;
			volatile uint8_t DataDirection;
			volatile uint8_t space4;
			volatile uint8_t InterruptEnableA;
			volatile uint8_t space5;
			volatile uint8_t InterruptEnableB;
			volatile uint8_t space6;
			volatile uint8_t InterruptPendingA;
			volatile uint8_t space7;
			volatile uint8_t InterruptPendingB;
			volatile uint8_t space8;
			volatile uint8_t InterruptInServiceA;
			volatile uint8_t space9;
			volatile uint8_t InterruptInServiceB;
			volatile uint8_t space10;
			volatile uint8_t InterruptMaskA;
			volatile uint8_t space11;
			volatile uint8_t InterruptMaskB;
			volatile uint8_t space12;
			volatile uint8_t InterruptVector;
			volatile uint8_t space13;
			volatile uint8_t TimerControlA;
			volatile uint8_t space14;
			volatile uint8_t TimerControlB;
			volatile uint8_t space15;
			volatile uint8_t TimerControlCD;
			volatile uint8_t space16;
			volatile uint8_t TimerDataA;
			volatile uint8_t space17;
			volatile uint8_t TimerDataB;
			volatile uint8_t space18;
			volatile uint8_t TimerDataC;
			volatile uint8_t space19;
			volatile uint8_t TimerDataD;
			volatile uint8_t space20;
			volatile uint8_t SyncChar;
			volatile uint8_t space21;
			volatile uint8_t SerialControl;
			volatile uint8_t space22;
			volatile uint8_t ReceiverStatus;
			volatile uint8_t space23;
			volatile uint8_t TransmitterStatus;
			volatile uint8_t space24;
			volatile uint8_t SerialData;
		} PACKED;

		inline static _mfp& _m(void) { return(*(_mfp*)0xfffa00); };

		static const uint8_t GPIP_COLOR = 0x80;

	public:
		inline static bool IsColorMonitor(void)
		{
			return((_m().GpipData & GPIP_COLOR) == GPIP_COLOR);
		}
	};
}

#endif
