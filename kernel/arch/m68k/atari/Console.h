// Console.h - Atari console driver.

#ifndef __ARCH_M68K_ATARI_CONSOLE_H__
#define __ARCH_M68K_ATARI_CONSOLE_H__

#include <Console.h>

namespace Kernel
{
	namespace Console
	{
		extern unsigned int numColumns;
		extern unsigned int numLines;

		void Init(void);

		inline unsigned int GetColumns(void)
		{
			return numColumns;
		}

		inline unsigned int GetLines(void)
		{
			return numLines;
		}

		inline void MoveCursor()
		{
		}
	}
}

#endif
