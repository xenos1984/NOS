// Console.cpp - VGA text mode console driver.

#include <cstdint>
#include <Symbol.h>
#include <Console.h>
#include INC_ARCH(Port.h)

namespace Kernel
{
	namespace Console
	{
		union Character
		{
			struct
			{
				uint8_t character;
				uint8_t attribute;
			} ch_at;
			uint16_t raw;
		};

		static const uintptr_t COLOR_VIDEO = 0xB8000; /**< Physical address of color video memory. */
		static const uintptr_t MONO_VIDEO = 0xB0000; /**< Physical address of monochrome video memory. */

		static const uint16_t CRTC_ADDR = 0x3d4;
		static const uint16_t CRTC_DATA = 0x3d5;
		static const uint16_t DEBUG_PORT = 0xe9;
		static const uint16_t DEBUG_PORT_VBOX = 0x504;

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

		static uint8_t attrib; /**< Text attribute byte. */

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

		void SetColor(unsigned char color)
		{
			char buf[10];

			attrib = (color & 0x2a) | (color & 0x11) << 2 | (color & 0x44) >> 2;
			ColorToAnsi(buf, color);

			for(char* p = buf; *p; p++)
			{
				Port::WriteU8(DEBUG_PORT, *p);
				Port::WriteU8(DEBUG_PORT_VBOX, *p);
			}
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

			if(c != '\n' && c != '\r')
			{
				Port::WriteU8(DEBUG_PORT, c); // Bochs / QEMU debugger console
				Port::WriteU8(DEBUG_PORT_VBOX, c); // VirtualBox debugger console

				if(c == '\t')
				{
					xPos += (8 - (xPos & 7));
				}
				else
				{
					videoMemoryCh()[xPos + yPos * COLUMNS].ch_at.character = c;
					videoMemoryCh()[xPos + yPos * COLUMNS].ch_at.attribute = attrib;
					buffer.ch_at[(bufoffset + xPos + yPos * COLUMNS) % TOTAL_CHARS].ch_at.character = c;
					buffer.ch_at[(bufoffset + xPos + yPos * COLUMNS) % TOTAL_CHARS].ch_at.attribute = attrib;
					xPos++;
				}
				if(xPos < COLUMNS)
					return;
			}

			Port::WriteU8(DEBUG_PORT, '\n');
			Port::WriteU8(DEBUG_PORT_VBOX, '\n');
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
