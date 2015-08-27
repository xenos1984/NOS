// Console.cpp - Atari console driver.

#include <cstddef>
#include <Console.h>
#include <Fonts.h>
#include INC_SUBARCH(Video.h)
#include INC_SUBARCH(Blitter.h)
#include INC_SUBARCH(MFP.h)
#include INC_SUBARCH(SysVars.h)

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

		unsigned int numColumns;
		unsigned int numLines;
		static unsigned int numPlanes; /**< Number of bit planes. */
		static unsigned int bytesPerLine; /**< Bytes per line. */
		static unsigned int totalBytes; /**< Total length of video memory. */
		static unsigned int height;
		static unsigned int xPos; /**< Cursor position (line). */
		static unsigned int yPos; /**< Cursor position (column). */
		static volatile uint8_t* videoMemory; /**< Pointer to video memory. */
		static const uint8_t* font;

		static const Attribute attribs4[] =
		{
			static_cast<Attribute>(BG_BLACK | FG_LIGHT_GRAY),
			static_cast<Attribute>(BG_BLACK | FG_WHITE),
			static_cast<Attribute>(BG_BLACK | FG_GREEN),
			static_cast<Attribute>(BG_BLACK | FG_YELLOW),
			static_cast<Attribute>(BG_BLACK | FG_RED)
		};
		static const Attribute attribs1[] =
		{
			static_cast<Attribute>(BG_BLACK | FG_WHITE),
			static_cast<Attribute>(BG_BLACK | FG_WHITE),
			static_cast<Attribute>(BG_BLACK | FG_WHITE),
			static_cast<Attribute>(BG_BLACK | FG_WHITE),
			static_cast<Attribute>(BG_BLACK | FG_WHITE)
		};
		static Attribute attrib; /**< Text attribute byte. */

		void SECTION(".init.text") Init(void)
		{
			if(MFP::IsColorMonitor())
			{
				numLines = 25;
				numColumns = 40;
				numPlanes = 4;
				height = 8;
				font = font8x8t;
				attrib  = attribs4[0];
			}
			else
			{
				numLines = 25;
				numColumns = 80;
				numPlanes = 1;
				height = 16;
				font = font8x16t;
				attrib  = attribs1[0];
			}
			xPos = yPos = 0;
			bytesPerLine = height * numPlanes * numColumns;
			totalBytes = numLines * bytesPerLine;

			videoMemory = (volatile uint8_t*)sysvars.VideoBase;
		}

		void SetStyle(Style s)
		{
			attrib = (numPlanes == 4 ? attribs4 : attribs1)[static_cast<int>(s)];
		}

		void Clear(void)
		{
			uint16_t src[4];
			unsigned int i;

			for(i = 0; i < numPlanes; i++)
				src[i] = -(attrib >> ((4 + i)) & 1);

			Blitter::BitBlt(true, (void*)src, (void*)videoMemory, 2, -2 * (numPlanes - 1), 2, 2, numPlanes, height * numLines * numColumns / 2);
			xPos = yPos = 0;
		}

		void PutChar(unsigned char c)
		{
			unsigned int i, j;

			if(c == '\t')
			{
				xPos += (8 - (xPos & 7));
				if(xPos < numColumns)
					return;
			}
			else if(c != '\n' && c != '\r')
			{
				for(i = 0; i < height; i++)
					for(j = 0; j < numPlanes; j++)
						videoMemory[(xPos & 1) + (xPos & ~1) * numPlanes + (yPos * height + i) * numColumns * numPlanes + 2 * j] = ((0xff * ((attrib >> j) & 1)) & font[256 * i + c]) | ((0xff * ((attrib >> (j + 4)) & 1)) & ~font[256 * i + c]);

				xPos++;
				if(xPos < numColumns)
					return;
			}

			xPos = 0;
			yPos++;
			if(yPos >= numLines)
			{
				yPos = numLines - 1;

				// Scroll up a line.
				Blitter::BitBlt(true, (void*)(videoMemory + bytesPerLine), (void*)videoMemory, 2, 2, 2, 2, numColumns * numPlanes / 2, height * (numLines - 1));

				// Clear the last line.
				uint16_t src[4];

				for(i = 0; i < numPlanes; i++)
					src[i] = -(attrib >> ((4 + i)) & 1);

				Blitter::BitBlt(true, (void*)src, (void*)(videoMemory + totalBytes - bytesPerLine), 2, -2 * (numPlanes - 1), 2, 2, numPlanes, height * numColumns / 2);
			}
		}
	}
}
