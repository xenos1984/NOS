// VGAConsole.h - VGA console driver for the HAL.

#ifndef __ARCH_X86_VGACONSOLE_H__
#define __ARCH_X86_VGACONSOLE_H__

#include <cstdint>
#include <Console.h>
#include <Symbol.h>

namespace Kernel
{
	/** VGA text mode console.
	  * Used for standard VGA text mode output.
	  */
	class VGAConsole : public Console
	{
	private:
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

		static const int LINES = 25;
		static const int COLUMNS = 80;
		static const int BYTES_PER_LINE = 2 * COLUMNS; /**< Bytes per line. */
		static const int TOTAL_BYTES = LINES * BYTES_PER_LINE; /**< Total length of video memory. */

		int xPos; /**< Cursor position (line). */
		int yPos; /**< Cursor position (column). */
		uint8_t attrib; /**< Text attribute byte. */

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

	protected:
		void putChar(unsigned char c); /**< Put character on the screen. */
		void moveCursor(void); /**< Set cursor position. */

	public:
		VGAConsole(void);
		~VGAConsole(void);

		void Clear(void);

		uint8_t GetBackground(void);
		void SetBackground(uint8_t value);

		uint8_t GetForeground(void);
		void SetForeground(uint8_t value);
	};
}

#endif
