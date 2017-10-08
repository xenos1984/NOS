// Mailbox.cpp - Raspberry Pi Mailbox

#include INC_VENDOR(Mailbox.h)
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
			Send(1, 8, ((uintptr_t)&buffer) - 0xc0000000);
			Receive(0, 8);
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
			Send(1, CH_ARM_VID, ((uintptr_t)&buffer) - 0xc0000000);
			Receive(0, CH_ARM_VID);
/*
			for(unsigned int i = 0; i < sizeof(buffer) >> 2; i++)
				Console::WriteFormat("Buffer[%d] = 0x%8x\n", i, ((uint32_t*)&buffer)[i]);
*/
			return buffer.GetTags().GetData();
		}

		uint32_t GetFirmwareRev(void)
		{
			return QueryMailboxProp<uint32_t>(PropTags::TAG_FW_REV);
		}

		uint32_t GetBoardModel(void)
		{
			return QueryMailboxProp<uint32_t>(PropTags::TAG_BOARD_MODEL);
		}

		uint32_t GetBoardRev(void)
		{
			return QueryMailboxProp<uint32_t>(PropTags::TAG_BOARD_REV);
		}

		MemInfo GetArmMemory(void)
		{
			return QueryMailboxProp<MemInfo>(PropTags::TAG_ARM_MEM);
		}

		MemInfo GetVideoMemory(void)
		{
			return QueryMailboxProp<MemInfo>(PropTags::TAG_VIDEO_MEM);
		}
	}
}
