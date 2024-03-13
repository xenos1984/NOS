// Mailbox.cpp - Raspberry Pi Mailbox

#include INC_PLATFORM(Mailbox.h)
#include <Pager.h>
#include <Console.h>

namespace Kernel
{
	namespace Mailbox
	{
		void Send(unsigned int mb, Channel ch, uint32_t value)
		{
			while(IsFull(mb)) ;
			Write(mb, REG_DATA, value | ch);
		}

		uint32_t Receive(unsigned int mb, Channel ch)
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

		template<typename Tin, typename Tout> Tout QueryMailboxProp(PropTags tag, const Tin& tin)
		{
			PropertyBuffer<PropertyTag<Tin, Tout>> buffer = PropertyBuffer<PropertyTag<Tin, Tout>>(PropertyTag<Tin, Tout>(tag, tin));
/*
			for(int i = 0; i < sizeof(buffer) >> 2; i++)
				Console::WriteFormat("Buffer[%d] = 0x%8x\n", i, ((uint32_t*)&buffer)[i]);
*/
			Send(1, CH_ARM_VID, Pager::VirtToPhys(&buffer));
			Receive(0, CH_ARM_VID);
/*
			for(int i = 0; i < sizeof(buffer) >> 2; i++)
				Console::WriteFormat("Buffer[%d] = 0x%8x\n", i, ((uint32_t*)&buffer)[i]);
*/
			return buffer.GetTags().GetData();
		}

		template<typename Tout> Tout QueryMailboxProp(PropTags tag)
		{
			PropertyBuffer<PropertyTag<void, Tout>> buffer = PropertyBuffer<PropertyTag<void, Tout>>(PropertyTag<void, Tout>(tag));
/*
			for(unsigned int i = 0; i < sizeof(buffer) >> 2; i++)
				Console::WriteFormat("Buffer[%d] = 0x%8x\n", i, ((uint32_t*)&buffer)[i]);
*/
			Send(1, CH_ARM_VID, Pager::VirtToPhys(&buffer));
			Receive(0, CH_ARM_VID);
/*
			for(unsigned int i = 0; i < sizeof(buffer) >> 2; i++)
				Console::WriteFormat("Buffer[%d] = 0x%8x\n", i, ((uint32_t*)&buffer)[i]);
*/
			return buffer.GetTags().GetData();
		}

		template uint32_t QueryMailboxProp<uint32_t>(PropTags tag);
		template MemInfo QueryMailboxProp<MemInfo>(PropTags tag);
	}
}
