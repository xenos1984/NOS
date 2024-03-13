// UART.h - Raspberry Pi serial UART

#ifndef __PLATFORM_RASPI_UART_H__
#define __PLATFORM_RASPI_UART_H__

#include <cstdint>
#include INC_PLATFORM(Entry.h)

namespace Kernel
{
	namespace UART
	{
		static const uintptr_t base_addr = PERI_OFFSET + 0x00201000;

		enum Register : int
		{
			REG_DR     = 0x00 / 4,
			REG_RSRECR = 0x04 / 4,
			REG_FR     = 0x18 / 4,
			REG_ILPR   = 0x20 / 4,
			REG_IBRD   = 0x24 / 4,
			REG_FBRD   = 0x28 / 4,
			REG_LCRH   = 0x2c / 4,
			REG_CR     = 0x30 / 4,
			REG_IFLS   = 0x34 / 4,
			REG_IMSC   = 0x38 / 4,
			REG_RIS    = 0x3c / 4,
			REG_MIS    = 0x40 / 4,
			REG_ICR    = 0x44 / 4,
			REG_DMACR  = 0x48 / 4,
			REG_ITCR   = 0x80 / 4,
			REG_ITIP   = 0x84 / 4,
			REG_ITOP   = 0x88 / 4,
			REG_TDR    = 0x8c / 4
		};

		enum Flags : uint32_t
		{
			FR_BUSY = 0x08,
			FR_RXFE = 0x10,
			FR_TXFF = 0x20,
			FR_RXFF = 0x40,
			FR_TXFE = 0x80
		};

		enum Control : uint32_t
		{
			CR_UARTEN = 0x0001,
			CR_LBE    = 0x0080,
			CR_TXE    = 0x0100,
			CR_RXE    = 0x0200
		};

		enum LineControl : uint32_t
		{
			LCRH_BRK    = 0x01,
			LCRH_PEN    = 0x02,
			LCRH_EPS    = 0x04,
			LCRH_STP_1  = 0x00,
			LCRH_STP_2  = 0x08,
			LCRH_FEN    = 0x10,
			LCRH_WLEN_5 = 0x00,
			LCRH_WLEN_6 = 0x20,
			LCRH_WLEN_7 = 0x40,
			LCRH_WLEN_8 = 0x60,
			LCRH_SPS    = 0x80
		};

		inline void Write(int reg, uint32_t value)
		{
			((volatile uint32_t*)base_addr)[reg] = value;
		}

		inline uint32_t Read(int reg)
		{
			return ((volatile uint32_t*)base_addr)[reg];
		}

		inline void SendChar(unsigned char c)
		{
			while(Read(REG_FR) & FR_TXFF) ;
			Write(REG_DR, c);
		}

		inline void Disable(void)
		{
			Write(REG_CR, 0);
		}

		inline void Enable(void)
		{
			Write(REG_CR, CR_UARTEN | CR_RXE | CR_TXE);
		}

		inline void EnableTx(void)
		{
			Write(REG_CR, CR_UARTEN | CR_TXE);
		}

		inline void EnableRx(void)
		{
			Write(REG_CR, CR_UARTEN | CR_RXE);
		}

		inline void ClearIRQ(void)
		{
			Write(REG_ICR, 0x7ff);
		}

		void Init(void);
		void SetBaud(unsigned long baud);
	}
}

#endif
