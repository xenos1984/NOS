// Console.cpp - VGA text mode console driver.

#include <cstdint>
#include <Symbol.h>
#include <Console.h>
#include INC_ARCH(Port.h)

namespace Kernel
{
	namespace Console
	{
		enum Attribute : uint8_t
		{
			FG_BLACK         = 0x00,
			FG_BLUE          = 0x01,
			FG_GREEN         = 0x02,
			FG_CYAN          = 0x03,
			FG_RED           = 0x04,
			FG_MAGENTA       = 0x05,
			FG_BROWN         = 0x06,
			FG_LIGHT_GRAY    = 0x07,
			FG_GRAY          = 0x08,
			FG_LIGHT_BLUE    = 0x09,
			FG_LIGHT_GREEN   = 0x0a,
			FG_LIGHT_CYAN    = 0x0b,
			FG_LIGHT_RED     = 0x0c,
			FG_LIGHT_MAGENTA = 0x0d,
			FG_YELLOW        = 0x0e,
			FG_WHITE         = 0x0f,

			BG_BLACK         = FG_BLACK      << 4,
			BG_BLUE          = FG_BLUE       << 4,
			BG_GREEN         = FG_GREEN      << 4,
			BG_CYAN          = FG_CYAN       << 4,
			BG_RED           = FG_RED        << 4,
			BG_MAGENTA       = FG_MAGENTA    << 4,
			BG_BROWN         = FG_BROWN      << 4,
			BG_LIGHT_GRAY    = FG_LIGHT_GRAY << 4
		};

		union Character
		{
			struct
			{
				uint8_t character;
				Attribute attribute;
			} ch_at;
			uint16_t raw;
		};

		static const uintptr_t COLOR_VIDEO = 0xB8000; /**< Physical address of color video memory. */
		static const uintptr_t MONO_VIDEO = 0xB0000; /**< Physical address of monochrome video memory. */

		static const uint16_t CRTC_ADDR = 0x3d4;
		static const uint16_t CRTC_DATA = 0x3d5;
		static const uint16_t DEBUG_PORT = 0xe9;

		static const unsigned int LINES = 25;
		static const unsigned int COLUMNS = 80;
		static const int BYTES_PER_LINE = 2 * COLUMNS; /**< Bytes per line. */
		static const int TOTAL_BYTES = LINES * BYTES_PER_LINE; /**< Total length of video memory. */
		static const int TOTAL_CHARS = LINES * COLUMNS; /**< Total number of characters. */

		static unsigned int xPos; /**< Cursor position (line). */
		static unsigned int yPos; /**< Cursor position (column). */
		static unsigned int bufoffset; /**< Current offset between video memory and cache. */

		static union
		{
			Character ch_at[TOTAL_CHARS];
			unsigned long raw[TOTAL_BYTES / sizeof(long)];
		} buffer; /**< Cache to avoid reading video memory when scrolling. */

		static const Attribute attribs[] =
		{
			static_cast<Attribute>(BG_BLACK | FG_LIGHT_GRAY),
			static_cast<Attribute>(BG_BLACK | FG_WHITE),
			static_cast<Attribute>(BG_BLACK | FG_GREEN),
			static_cast<Attribute>(BG_BLACK | FG_YELLOW),
			static_cast<Attribute>(BG_BLACK | FG_RED)
		};
		static Attribute attrib = attribs[0]; /**< Text attribute byte. */

		inline unsigned long mkpat(unsigned long pat, int len)
		{
			return (len == 0 ? pat : mkpat((pat << 16) | attrib, len - 1));
		}

		inline unsigned long clearpat(void)
		{
			return mkpat(0, sizeof(long) / 2) << 8;
		}

		/** Pointer to video memory as Character. */
		inline volatile Character* videoMemoryCh(void)
		{
			return (volatile Character*)(COLOR_VIDEO + Symbol::kernelOffset.Addr());
		}

		/** Pointer to video memory as unsigned long. */
		inline volatile unsigned long* videoMemoryLong(void)
		{
			return (volatile unsigned long*)(COLOR_VIDEO + Symbol::kernelOffset.Addr());
		}

		void SetStyle(Style s)
		{
			attrib = attribs[static_cast<int>(s)];
		}

		void Clear(void)
		{
			unsigned int length = TOTAL_BYTES / sizeof(long);
			unsigned long pattern = clearpat();

			while(length--)
			{
				videoMemoryLong()[length] = pattern;
				buffer.raw[length] = pattern;
			}
			xPos = yPos = 0;
			bufoffset = 0;
		}

		// Put the character C on the screen.
		void PutChar(unsigned char c)
		{
			volatile unsigned long *t;
			unsigned int i;

			Port::WriteU8(DEBUG_PORT, c); // Bochs debugger console

			if(c == '\t')
			{
				xPos += (8 - (xPos & 7));
				if(xPos < COLUMNS)
					return;
			}
			else if(c != '\n' && c != '\r')
			{
				videoMemoryCh()[xPos + yPos * COLUMNS].ch_at.character = c;
				videoMemoryCh()[xPos + yPos * COLUMNS].ch_at.attribute = attrib;
				buffer.ch_at[(bufoffset + xPos + yPos * COLUMNS) % TOTAL_CHARS].ch_at.character = c;
				buffer.ch_at[(bufoffset + xPos + yPos * COLUMNS) % TOTAL_CHARS].ch_at.attribute = attrib;

				xPos++;
				if(xPos < COLUMNS)
					return;
			}

			xPos = 0;
			yPos++;
			if(yPos >= LINES)
			{
				// Scroll up a line.
				bufoffset += COLUMNS;
				unsigned int ofs = 2 * bufoffset / sizeof(long);
				unsigned int total = TOTAL_BYTES / sizeof(long);
				unsigned long pattern = clearpat();

				yPos = LINES - 1;
				t = videoMemoryLong();
				for(i = 0; i < (TOTAL_BYTES - BYTES_PER_LINE) / sizeof(long); i++)
					t[i] = buffer.raw[(i + ofs) % total];
				// Clear the last line.
				for(; i < TOTAL_BYTES / sizeof(long); i++)
				{
					t[i] = buffer.raw[(i + ofs) % total] = pattern;
				}
			}
		}

		void MoveCursor(void)
		{
			uint16_t chrOffset = xPos + yPos * COLUMNS;

			Port::WriteU8(CRTC_ADDR, 14);
			Port::WriteU8(CRTC_DATA, chrOffset >> 8);
			Port::WriteU8(CRTC_ADDR, 15);
			Port::WriteU8(CRTC_DATA, chrOffset);
		}
	}
}
