// Console.cpp - Raspberry Pi serial console driver.

#include <Console.h>
#include INC_VENDOR(UART.h)

namespace Kernel
{
	namespace Console
	{/*
		void SECTION(".init.text") Init(void)
		{
			UART::Init();
		}
*/
		void SetStyle(Style s __attribute__((unused)))
		{
		}

		void Clear(void)
		{
		}

		void PutChar(unsigned char c)
		{
			UART::SendChar(c);
		}

		void MoveCursor(void)
		{
		}
	}
}
