// Mailbox.h - Raspberry Pi Mailbox

#ifndef __VENDOR_RASPI_MAILBOX_H__
#define __VENDOR_RASPI_MAILBOX_H__

#include <cstdint>
#include <Console.h>
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

		inline void Write(unsigned int mb, int reg, uint32_t value)
		{
			((volatile uint32_t*)base_addr)[(mb << 3) + reg] = value;
		}

		inline uint32_t Read(unsigned int mb, int reg)
		{
			return ((volatile uint32_t*)base_addr)[(mb << 3) + reg];
		}

		inline bool IsFull(unsigned int mb)
		{
			return (Read(mb, REG_STATUS) & MB_FULL);
		}

		inline bool IsEmpty(unsigned int mb)
		{
			return (Read(mb, REG_STATUS) & MB_EMPTY);
		}

		void Send(unsigned int mb, unsigned int ch, uint32_t value);
		uint32_t Receive(unsigned int mb, unsigned int ch);

		enum class PropTags : uint32_t
		{
			TAG_FW_REV      = 0x00000001,
			TAG_BOARD_MODEL = 0x00010001,
			TAG_BOARD_REV   = 0x00010002
		};

		struct PropBufHeader
		{
			uint32_t size;
			uint32_t cmd;
		};

		struct PropTagHeader
		{
			PropTags tag;
			uint32_t bufsize;
			uint32_t datasize;
		};

		template<PropTags tag, typename Tin, typename Tout> class PropertyTag
		{
		private:
			PropTagHeader header;
			union
			{
				Tin din;
				Tout dout;
			};

		public:
			PropertyTag(const Tin& data)
			{
				din = data;
				header.tag = tag;
				header.bufsize = sizeof(Tout);
				header.datasize = sizeof(Tin);
			}

			const Tout& GetData(void) const
			{
				return dout;
			}
		};

		template<PropTags tag, typename Tout> class PropertyTag<tag, void, Tout>
		{
		private:
			PropTagHeader header;
			Tout dout;

		public:
			PropertyTag(void)
			{
				header.tag = tag;
				header.bufsize = sizeof(Tout);
				header.datasize = 0;
			}

			const Tout& GetData(void) const
			{
				return dout;
			}
		};

		template<typename T> class alignas(16) PropertyBuffer
		{
		private:
			PropBufHeader header;
			T tags;
			uint32_t terminator;

		public:
			enum RequestResponse : uint32_t
			{
				CMD_REQ =          0x00000000,
				CMD_RESP_SUCCESS = 0x80000000,
				CMD_RESP_ERROR   = 0x80000001
			};

			PropertyBuffer(const T& t)
			{
				header.size = sizeof(T) + 12;
				header.cmd = CMD_REQ;
				tags = t;
				terminator = 0;
			}

			const T& GetTags(void) const
			{
				return tags;
			}
		};

		template<PropTags tag, typename Tin, typename Tout> Tout QueryMailboxProp(const Tin& tin);
		template<PropTags tag, typename Tout> Tout QueryMailboxProp(void);

		uint32_t GetBoardModel(void);
	}
}

#endif
