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

		enum Channel : unsigned int
		{
			CH_POWER    = 0x0,
			CH_FRAMEBUF = 0x1,
			CH_VUART    = 0x2,
			CH_VCHIQ    = 0x3,
			CH_LED      = 0x4,
			CH_BUTTON   = 0x5,
			CH_TOUCHSCR = 0x6,
			CH_ARM_VID  = 0x8,
			CH_VID_ARM  = 0x9
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

		void Send(unsigned int mb, Channel ch, uint32_t value);
		uint32_t Receive(unsigned int mb, Channel ch);

		enum class PropTags : uint32_t
		{
			TAG_FW_REV       = 0x00000001,
			TAG_BOARD_MODEL  = 0x00010001,
			TAG_BOARD_REV    = 0x00010002,
			TAG_MAC_ADDR     = 0x00010003,
			TAG_BOARD_SERIAL = 0x00010004,
			TAG_ARM_MEM      = 0x00010005,
			TAG_VIDEO_MEM    = 0x00010006,
			TAG_CLOCKS       = 0x00010007,
			TAG_CMDLINE      = 0x00050001,
			TAG_DMA_CHANS    = 0x00060001,
			TAG_GET_POWER    = 0x00020001,
			TAG_TIMING       = 0x00020002,
			TAG_SET_POWER    = 0x00028001,
			TAG_GET_CLK_ST   = 0x00030001,
			TAG_SET_CLK_ST   = 0x00038001,
			TAG_GET_CLK_RATE = 0x00030002,
			TAG_SET_CLK_RATE = 0x00038002,
			TAG_MAX_CLK_RATE = 0x00030004,
			TAG_MIN_CLK_RATE = 0x00030007,
			TAG_GET_TURBO    = 0x00030009,
			TAG_SET_TURBO    = 0x00038009,
			TAG_GET_VOLTAGE  = 0x00030003,
			TAG_SET_VOLTAGE  = 0x00038003,
			TAG_MAX_VOLTAGE  = 0x00030005,
			TAG_MIN_VOLTAGE  = 0x00030008,
			TAG_GET_TEMP     = 0x00030006,
			TAG_MAX_TEMP     = 0x0003000a,
			TAG_ALLOC_MEM    = 0x0003000c,
			TAG_LOCK_MEM     = 0x0003000d,
			TAG_UNLOCK_MEM   = 0x0003000e,
			TAG_RELEASE_MEM  = 0x0003000f,
			TAG_EXEC_CODE    = 0x00030010,
			TAG_DISPMANX     = 0x00030014,
			TAG_EDID_BLOCK   = 0x00030020,
			TAG_ALLOC_FBUF   = 0x00040001,
			TAG_RELEASE_FBUF = 0x00048001,
			TAG_GET_PHYS_WH  = 0x00040003,
			TAG_TST_PHYS_WH  = 0x00044003,
			TAG_SET_PHYS_WH  = 0x00048003,
			TAG_GET_VIRT_WH  = 0x00040004,
			TAG_TST_VIRT_WH  = 0x00044004,
			TAG_SET_VIRT_WH  = 0x00048004,
			TAG_GET_DEPTH    = 0x00040005,
			TAG_TST_DEPTH    = 0x00044005,
			TAG_SET_DEPTH    = 0x00048005,
			TAG_GET_PIX_ORD  = 0x00040006,
			TAG_TST_PIX_ORD  = 0x00044006,
			TAG_SET_PIX_ORD  = 0x00048006,
			TAG_GET_ALPHA    = 0x00040007,
			TAG_TST_ALPHA    = 0x00044007,
			TAG_SET_ALPHA    = 0x00048007,
			TAG_GET_PITCH    = 0x00040008,
			TAG_GET_VOFFS    = 0x00040009,
			TAG_TST_VOFFS    = 0x00044009,
			TAG_SET_VOFFS    = 0x00048009,
			TAG_GET_OVERSCAN = 0x0004000a,
			TAG_TST_OVERSCAN = 0x0004400a,
			TAG_SET_OVERSCAN = 0x0004800a,
			TAG_GET_PALETTE  = 0x0004000b,
			TAG_TST_PALETTE  = 0x0004400b,
			TAG_SET_PALETTE  = 0x0004800b,
			TAG_CURSOR_INFO  = 0x00008010,
			TAG_CURSOR_STATE = 0x00008011
		};

		enum VoltageID : uint32_t
		{
			VOLT_CORE    = 0x00000001,
			VOLT_SDRAM_C = 0x00000002,
			VOLT_SDRAM_P = 0x00000003,
			VOLT_SDRAM_I = 0x00000004
		};

		enum ClockID : uint32_t
		{
			CLK_EMMC  = 0x00000001,
			CLK_UART  = 0x00000002,
			CLK_ARM   = 0x00000003,
			CLK_CORE  = 0x00000004,
			CLK_V3D   = 0x00000005,
			CLK_H264  = 0x00000006,
			CLK_ISP   = 0x00000007,
			CLK_SDRAM = 0x00000008,
			CLK_PIXEL = 0x00000009,
			CLK_PWM   = 0x0000000a
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

		template<typename Tin, typename Tout> class PropertyTag
		{
		private:
			PropTagHeader header;
			union
			{
				Tin din;
				Tout dout;
			};

		public:
			PropertyTag(PropTags tag, const Tin& data) : din(data)
			{
				header.tag = tag;
				header.bufsize = sizeof(Tout);
				header.datasize = sizeof(Tin);
			}

			PropertyTag() = delete;
			PropertyTag(const PropertyTag<Tin, Tout>&) = default;

			const Tout& GetData(void) const
			{
				return dout;
			}
		};

		template<typename Tout> class PropertyTag<void, Tout>
		{
		private:
			PropTagHeader header;
			Tout dout;

		public:
			PropertyTag(PropTags tag)
			{
				header.tag = tag;
				header.bufsize = sizeof(Tout);
				header.datasize = 0;
			}

			PropertyTag() = delete;
			PropertyTag(const PropertyTag<void, Tout>&) = default;

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

			PropertyBuffer(const T& t) : tags(t)
			{
				header.size = sizeof(T) + 12;
				header.cmd = CMD_REQ;
				terminator = 0;
			}

			const T& GetTags(void) const
			{
				return tags;
			}

			RequestResponse GetStatus(void) const
			{
				return header.cmd;
			}
		};

		template<typename Tin, typename Tout> Tout QueryMailboxProp(PropTags tag, const Tin& tin);
		template<typename Tout> Tout QueryMailboxProp(PropTags tag);

		uint32_t GetFirmwareRev(void);
		uint32_t GetBoardModel(void);
		uint32_t GetBoardRev(void);

		struct MemInfo
		{
			uint32_t start;
			uint32_t length;
		};

		MemInfo GetArmMemory(void);
		MemInfo GetVideoMemory(void);
	}
}

#endif
