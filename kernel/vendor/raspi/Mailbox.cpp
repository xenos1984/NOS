// Mailbox.cpp - Raspberry Pi Mailbox

#include INC_VENDOR(Mailbox.h)
#include <Pager.h>

namespace Kernel
{
	namespace Mailbox
	{
		void Send(int mb, int ch, uint32_t value)
		{
			while(IsFull(mb)) ;
			Write(mb, REG_DATA, value | ch);
		}

		uint32_t Receive(int mb, int ch)
		{
			uint32_t data;

			while(true)
			{
				while(IsEmpty(mb)) ;
				data = Read(mb, REG_DATA);
				if((data & 0xf) == ch)
					return (data & 0xfffffff0);
			}
		}
	}
}
