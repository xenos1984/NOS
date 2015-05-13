// AtariConsole.h - Atari console driver.

#ifndef __ARCH_M68K_ATARI_ATARICONSOLE_H__
#define __ARCH_M68K_ATARI_ATARICONSOLE_H__

#include <Console.h>

namespace Kernel
{
	/** Atari "text mode" console.
	  */
	class AtariConsole : public Console
	{
	private:
		unsigned int numPlanes; /**< Number of bit planes. */
		unsigned int bytesPerLine; /**< Bytes per line. */
		unsigned int totalBytes; /**< Total length of video memory. */
		unsigned int xPos; /**< Cursor position (line). */
		unsigned int yPos; /**< Cursor position (column). */
		uint8_t attrib; /**< Text attribute byte. */
		volatile unsigned char* videoMemory; /**< Pointer to video memory. */
		const unsigned char* font;
		unsigned int height;

	protected:
		void putChar(unsigned char c); /**< Put character on the screen. */
		void moveCursor(); /**< Set cursor position. */

	public:
		AtariConsole(void);
		~AtariConsole(void);

		void Clear(void);

		uint8_t GetBackground(void);
		void SetBackground(uint8_t value);

		uint8_t GetForeground(void);
		void SetForeground(uint8_t value);
	};
}

#endif
