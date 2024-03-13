// Console.cpp - Raspberry Pi serial console driver.

#include <Console.h>
#include INC_PLATFORM(UART.h)

namespace Kernel
{
	namespace Console
	{/*
		void SECTION(".init.text") Init(void)
		{
			UART::Init();
		}
*/
		void SetColor(unsigned char color)
		{
			char buf[10];

			ColorToAnsi(buf, color);

			for(char* p = buf; *p; p++)
				UART::SendChar(*p);
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
