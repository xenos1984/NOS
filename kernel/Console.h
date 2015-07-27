// Console.h - Basic console driver for the HAL.

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <cstdarg>
#include <cstdint>
#include <AtomicOps.h>

namespace Kernel
{
	/** Abstract console class for textual output.
	  * The Console class provides basic functions to prompt messages to the user. It also offers some output
	  * formatting and colored output. Architecture specific consoles are derived from this class.
	  */
	class Console
	{
	private:
		AtomicLock lock;

		uint8_t msgback[4]; /**< Background colors for messages. */
		uint8_t msgfore[4]; /**< Foreground colors for messages. */
		uint8_t infoback; /**< Background color for information. */
		uint8_t infofore; /**< Foreground color for information. */

		/** Convert an integer to a formatted string.
		  * @param buf String to save the target in.
		  * @param divisor 10 for decimal output, 16 for hex output.
		  * @param number Integer to be converted.
		  * @param length Minimum length of the output, pad with 0 if necessary.
		  */
		template <class T> void itoa(char *buf, T divisor, T number, int length);

		/** Chainable printf function.
		  * Prints the arguments as given by the format string and returns a pointer to additional arguments.
		  * @param format The format string.
		  * @param arg Pointer to the list of arguments.
		  * @return Pointer to the next argument to be formatted and printed.
		  */
		void writef(const char* format, va_list* arg);

	protected:
		unsigned int numLines; /**< Number of lines. */
		unsigned int numColumns; /**< Number of columns. */

		virtual void putChar(unsigned char c) = 0; /**< Put character on the screen. */
		virtual void moveCursor() = 0; /**< Set cursor position. */

	public:
		static const uint8_t FG_BLACK         = 0x00;
		static const uint8_t FG_BLUE          = 0x01;
		static const uint8_t FG_GREEN         = 0x02;
		static const uint8_t FG_CYAN          = 0x03;
		static const uint8_t FG_RED           = 0x04;
		static const uint8_t FG_MAGENTA       = 0x05;
		static const uint8_t FG_BROWN         = 0x06;
		static const uint8_t FG_LIGHT_GRAY    = 0x07;
		static const uint8_t FG_GRAY          = 0x08;
		static const uint8_t FG_LIGHT_BLUE    = 0x09;
		static const uint8_t FG_LIGHT_GREEN   = 0x0a;
		static const uint8_t FG_LIGHT_CYAN    = 0x0b;
		static const uint8_t FG_LIGHT_RED     = 0x0c;
		static const uint8_t FG_LIGHT_MAGENTA = 0x0d;
		static const uint8_t FG_YELLOW        = 0x0e;
		static const uint8_t FG_WHITE         = 0x0f;

		static const uint8_t BG_BLACK         = FG_BLACK      << 4;
		static const uint8_t BG_BLUE          = FG_BLUE       << 4;
		static const uint8_t BG_GREEN         = FG_GREEN      << 4;
		static const uint8_t BG_CYAN          = FG_CYAN       << 4;
		static const uint8_t BG_RED           = FG_RED        << 4;
		static const uint8_t BG_MAGENTA       = FG_MAGENTA    << 4;
		static const uint8_t BG_BROWN         = FG_BROWN      << 4;
		static const uint8_t BG_LIGHT_GRAY    = FG_LIGHT_GRAY << 4;

		static const uint8_t MSG_INFO    = 0x00; /**< Information message. */
		static const uint8_t MSG_OK      = 0x01; /**< Success message. */
		static const uint8_t MSG_WARNING = 0x02; /**< Warning message. */
		static const uint8_t MSG_ERROR   = 0x03; /**< Error message. */

		Console(void);
		~Console(void);

		virtual void Clear(void) = 0; /**< Clear console device. */

		void Write(char chr); /**< Write single character. */
		void Write(const char* string); /**< Write string. */

		void WriteLine(const char* string); /**< Write string and start a new line. */

		void WriteFormat(const char *format, ...); /**< Write formatted output (aka 'printf') */

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
		void WriteMessage(uint8_t type, const char *message, const char *result, ...);

		virtual uint8_t GetBackground(void) = 0; /**< Get background color. */
		virtual void SetBackground(uint8_t value) = 0; /**< Set background color. */

		virtual uint8_t GetForeground(void) = 0; /**< Get foreground color. */
		virtual void SetForeground(uint8_t value) = 0; /**< Set foreground color. */

		inline int GetColumns(void)
		{
			return numColumns;
		}

		inline int GetLines(void)
		{
			return numLines;
		}
	};
}

/** Space for Console object. */
extern char console_space[];

/** Returns a reference to the console object. */
inline Kernel::Console& console(void) { return(reinterpret_cast<Kernel::Console&>(console_space)); }

#endif
