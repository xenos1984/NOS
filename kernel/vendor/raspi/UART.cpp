// UART.cpp - Raspberry Pi serial UART

#include INC_VENDOR(UART.h)
#include INC_VENDOR(GPIO.h)

namespace Kernel
{
	namespace UART
	{
		void SetBaud(unsigned long baud)
		{
			const uint32_t ibrd = 187500 / baud;
			const uint32_t fbrd = (12000000 + baud / 2) / baud - 64 * ibrd;

			Write(REG_IBRD, ibrd);
			Write(REG_FBRD, fbrd);
		}

		void Init(void)
		{
			// Disable UART.
			Disable();

			// Disable pull up / down.
			GPIO::TogglePull(14, GPIO::PULL_NONE);
			GPIO::TogglePull(15, GPIO::PULL_NONE);

			// Clear pending interrupts.
			Write(REG_ICR, 0x7ff);

			// Set baud rate.
			SetBaud(115200);

			// Enable FIFO & 8 bit transmission (1 stop bit, no parity).
			Write(REG_LCRH, LCRH_FEN | LCRH_WLEN_8);

			// Mask interrupts.
			Write(REG_IMSC, 0x7f2);

			// Enable UART.
			Enable();
		}
	}
}
