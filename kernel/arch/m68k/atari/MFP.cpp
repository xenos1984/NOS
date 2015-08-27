// MFP.cpp - Atari multi-function peripheral MC68901.

#include INC_SUBARCH(MFP.h)

using namespace Kernel;

SECTION(".init.text") void MFP::Init(void)
{
	_m().TimerControlCD = (_m().TimerControlCD & 0xf8) | 0x05;
	_m().SerialControl = 0x08;
	_m().ReceiverStatus = 0x01;
	_m().TransmitterStatus = 0x01;
}

void MFP::SerialSend(uint8_t c)
{
	while(_m().TransmitterStatus & 0x80 == 0)
		;
	_m().SerialData = c;
}
