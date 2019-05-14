// Console.h - VGA text mode console driver.

#ifndef __VENDOR_PC_CONSOLE_H__
#define __VENDOR_PC_CONSOLE_H__

namespace Kernel
{
	namespace Console
	{
		inline unsigned int GetColumns(void)
		{
			return 80;
		}

		inline unsigned int GetLines(void)
		{
			return 25;
		}
	}
}

#endif
