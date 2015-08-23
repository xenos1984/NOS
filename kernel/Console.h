// Console.h - Basic console driver for the HAL.

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <cstdarg>
#include <cstdint>

namespace Kernel
{
	/** Abstract console class for textual output.
	  * The Console class provides basic functions to prompt messages to the user. It also offers some output
	  * formatting and colored output. Architecture specific consoles are derived from this class.
	  */
	namespace Console
	{
		enum class Style
		{
			TEXT,
			INFO,
			OK,
			WARNING,
			ERROR
		};

		void PutChar(unsigned char c); /**< Put character on the screen. */
		void MoveCursor(void); /**< Set cursor position. */
		void Clear(void); /**< Clear console device. */
		void SetStyle(Style s);

		/**
		  * Prompts a formatted message to the user.
		  * The message contains of a message body and a result. Both of them are format strings as they are
		  * known from the C language 'printf' function. It may take additional parameters, just like 'printf'
		  * does. The message is then concatenated and formatted (colored) depending on the message type. A
		  * newline is automatically generated.
		  * @param type Message type (information, success, warning, error). Determines output color.
		  * @param message Message body format string, printed in default output color.
		  * @param result Message result format string, printed in type specific color.
		  * @param ... Additional arguments as in 'printf'.
		  */
		void WriteMessage(Style style, const char *message, const char *result, ...);
		void Write(char chr); /**< Write single character. */
		void Write(const char* string); /**< Write string. */
		void WriteLine(const char* string); /**< Write string and start a new line. */
		void WriteFormat(const char *format, ...); /**< Write formatted output (aka 'printf') */
	}
}

#include INC_ARCH(Console.h)

#endif
