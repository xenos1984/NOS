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

		template<PropTags tag, typename Tin, typename Tout> Tout QueryMailboxProp(const Tin& tin)
		{
			PropertyBuffer<PropertyTag<tag, Tin, Tout>> buffer = PropertyBuffer<PropertyTag<tag, Tin, Tout>>(PropertyTag<tag, Tin, Tout>(tin));

			for(int i = 0; i < sizeof(buffer) >> 2; i++)
				Console::WriteFormat("Buffer[%d] = 0x%8x\n", i, ((uint32_t*)&buffer)[i]);

			Send(1, 8, ((uintptr_t)&buffer) - 0xc0000000);
			Console::WriteFormat("Data received in mailbox: 0x%8x\n", Receive(0, 8));

			for(int i = 0; i < sizeof(buffer) >> 2; i++)
				Console::WriteFormat("Buffer[%d] = 0x%8x\n", i, ((uint32_t*)&buffer)[i]);

			return buffer.GetTags().GetData();
		}

		template<PropTags tag, typename Tout> Tout QueryMailboxProp(void)
		{
			PropertyBuffer<PropertyTag<tag, void, Tout>> buffer = PropertyBuffer<PropertyTag<tag, void, Tout>>(PropertyTag<tag, void, Tout>());

			for(unsigned int i = 0; i < sizeof(buffer) >> 2; i++)
				Console::WriteFormat("Buffer[%d] = 0x%8x\n", i, ((uint32_t*)&buffer)[i]);

			Send(1, 8, ((uintptr_t)&buffer) - 0xc0000000);
			Console::WriteFormat("Data received in mailbox: 0x%8x\n", Receive(0, 8));

			for(unsigned int i = 0; i < sizeof(buffer) >> 2; i++)
				Console::WriteFormat("Buffer[%d] = 0x%8x\n", i, ((uint32_t*)&buffer)[i]);

			return buffer.GetTags().GetData();
		}

		uint32_t GetBoardModel(void)
		{
			return QueryMailboxProp<PropTags::TAG_BOARD_MODEL, uint32_t>();
		}
	}
}
