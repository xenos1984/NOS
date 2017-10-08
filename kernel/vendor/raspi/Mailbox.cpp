// Mailbox.cpp - Raspberry Pi Mailbox

#include INC_VENDOR(Mailbox.h)
#include <Pager.h>
#include <Console.h>

namespace Kernel
{
	namespace Mailbox
	{
		void Send(unsigned int mb, unsigned int ch, uint32_t value)
		{
			while(IsFull(mb)) ;
			Write(mb, REG_DATA, value | ch);
		}

		uint32_t Receive(unsigned int mb, unsigned int ch)
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

		uint32_t GetBoardModel(void)
		{
			PropertyBuffer<PropTagBoardModel> buffer = PropertyBuffer<PropTagBoardModel>(PropTagBoardModel());

			for(int i = 0; i < 7; i++)
				Console::WriteFormat("Buffer[%d] = 0x%8x\n", i, ((uint32_t*)&buffer)[i]);

			Send(1, 8, ((uintptr_t)&buffer) - 0xc0000000);
			Console::WriteFormat("Data received in mailbox: 0x%8x\n", Receive(0, 8));

			for(int i = 0; i < 7; i++)
				Console::WriteFormat("Buffer[%d] = 0x%8x\n", i, ((uint32_t*)&buffer)[i]);

			return buffer.GetTags().GetData();
		}
	}
}
