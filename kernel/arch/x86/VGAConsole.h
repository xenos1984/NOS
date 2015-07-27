// VGAConsole.h - VGA console driver for the HAL.

#ifndef __ARCH_X86_VGACONSOLE_H__
#define __ARCH_X86_VGACONSOLE_H__

#include <cstdint>
#include <Console.h>

namespace Kernel
{
	/** VGA text mode console.
	  * Used for standard VGA text mode output.
	  */
	class VGAConsole : public Console
	{
	private:
		static const uint32_t COLOR_VIDEO = 0xB8000; /**< Physical address of color video memory. */
		static const uint32_t MONO_VIDEO = 0xB0000; /**< Physical address of monochrome video memory. */
		static const uint16_t CrtcAddr = 0x3d4;
		static const uint16_t CrtcData = 0x3d5;
		static const uint16_t DebugPort = 0xe9;

		unsigned int bytesPerLine; /**< Bytes per line. */
		unsigned int totalBytes; /**< Total length of video memory. */
		unsigned int xPos; /**< Cursor position (line). */
		unsigned int yPos; /**< Cursor position (column). */
		uint8_t attrib; /**< Text attribute byte. */
		volatile unsigned char *videoMemory; /**< Pointer to video memory. */

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
