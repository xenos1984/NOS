// Mailbox.h - Raspberry Pi Mailbox

#ifndef __VENDOR_RASPI_MAILBOX_H__
#define __VENDOR_RASPI_MAILBOX_H__

#include <cstdint>
#include INC_VENDOR(Entry.h)

namespace Kernel
{
	namespace Mailbox
	{
		static const uintptr_t base_addr = PERI_OFFSET + 0x0000b880;

		enum Register : int
		{
			REG_DATA   = 0x00 / 4,
			REG_PEEK   = 0x10 / 4,
			REG_SENDER = 0x14 / 4,
			REG_STATUS = 0x18 / 4,
			REG_CONFIG = 0x1c / 4
		};

		enum Status : uint32_t
		{
			MB_FULL  = 0x80000000,
			MB_EMPTY = 0x40000000
		};

		inline void Write(int mb, int reg, uint32_t value)
		{
			((volatile uint32_t*)base_addr)[(mb << 3) + reg] = value;
		}

		inline uint32_t Read(int mb, int reg)
		{
			return ((volatile uint32_t*)base_addr)[(mb << 3) + reg];
		}

		inline bool IsFull(int mb)
		{
			return (Read(mb, REG_STATUS) | MB_FULL);
		}

		inline bool IsEmpty(int mb)
		{
			return (Read(mb, REG_STATUS) | MB_EMPTY);
		}

		void Send(int mb, int ch, uint32_t value);
		uint32_t Receive(int mb, int ch);
	}
}

#endif
