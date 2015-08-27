// Console.cpp - Basic console driver for the kernel.

#include <cstdarg>
#include <cstdint>
#include <Console.h>
#include <AtomicOps.h>

namespace Kernel
{
	namespace Console
	{
		static AtomicLock lock;

		void Write(char chr)
		{
			PutChar(chr);
			MoveCursor();
		}

		void Write(const char *string)
		{
			char c;

			while((c = *string++) != 0)
				PutChar(c);
			MoveCursor();
		}

		void WriteLine(const char *string)
		{
			char c;

			lock.Enter();
			while((c = *string++) != 0)
				PutChar(c);

			// Advance to the next line.
			PutChar('\n');
			MoveCursor();
			lock.Exit();
		}

		/** Convert an integer to a formatted string.
		  * @param buf String to save the target in.
		  * @param divisor 10 for decimal output, 16 for hex output.
		  * @param number Integer to be converted.
		  * @param length Minimum length of the output, pad with 0 if necessary.
		  */
		template<class T> void itoa(char *buf, T divisor, T number, int length)
		{
			char *p = buf;
			char *p1, *p2;
			char tmp;
			T ud = number;
			T remainder;

			if(ud < 0)
			{
				*p++ = '-';
				buf++;
				ud = -ud;
			}

			do
			{
				remainder = ud % divisor;
				*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
				length--;
			}
			while(ud /= divisor);

			while(length > 0)
			{
				*p++ = '0';
				length--;
			}

			*p = 0;

			p1 = buf;
			p2 = p - 1;
			while(p1 < p2)
			{
				tmp = *p1;

				*p1 = *p2;
				*p2 = tmp;
				p1++;
				p2--;
			}
		}

		/** Chainable printf function.
		  * Prints the arguments as given by the format string and returns a pointer to additional arguments.
		  * @param format The format string.
		  * @param arg Pointer to the list of arguments.
		  * @return Pointer to the next argument to be formatted and printed.
		  */
		void writef(const char* format, va_list* arg)
		{
			char c;
			const char *p;
			char buf[24];
			int length;
			bool longword = false;

			// Repeat while string is not terminated.
			while((c = *format++) != 0)
			{
				length = 0;
				// Check whether we hit a format specifier.
				if(c != '%')
					Write(c);
				else
				{
					longword = false;
					c = *format++;
					// Numbers determine the output string length.
					while(c >= '0' && c <= '9')
					{
						length *= 10;
						length += c - '0';
						c = *format++;
					}
					// The l indicates that a 64 bit value is used as argument.
					if(c == 'l')
					{
						longword = true;
						c = *format++;
					}
					// Check which format is requested.
					switch(c)
					{
					case '%': // Double % escapes %.
						Write(c);
						continue;

					case 'o': // Print in octal, unsigned.
						if(longword)
							itoa<uint64_t>(buf, 8, va_arg(*arg, uint64_t), length);
						else
							itoa<uint32_t>(buf, 8, va_arg(*arg, uint32_t), length);
						p = buf;
						length = 0;
						break;

					case 'd': // Print in decimal, signed.
						if(longword)
							itoa<int64_t>(buf, 10, va_arg(*arg, int64_t), length);
						else
							itoa<int32_t>(buf, 10, va_arg(*arg, int32_t), length);
						p = buf;
						length = 0;
						break;

					case 'u': // Print in decimal, unsigned.
						if(longword)
							itoa<uint64_t>(buf, 10, va_arg(*arg, uint64_t), length);
						else
							itoa<uint32_t>(buf, 10, va_arg(*arg, uint32_t), length);
						p = buf;
						length = 0;
						break;

					case 'x': // Print in hexadecimal, unsigned.
						if(longword)
							itoa<uint64_t>(buf, 16, va_arg(*arg, uint64_t), length);
						else
							itoa<uint32_t>(buf, 16, va_arg(*arg, uint32_t), length);
						p = buf;
						length = 0;
						break;

					case 'p': // Interpret as pointer type and print address in hexadecimal.
						if(!length)
							length = 2 * sizeof(uintptr_t);
						itoa<uintptr_t>(buf, 16, va_arg(*arg, uintptr_t), length);
						p = buf;
						length = 0;
						break;

					case 'c': // Print as ASCII char.
						Write(va_arg(*arg, int));
						continue;

					case 's': // Print string from pointer.
						p = va_arg(*arg, char*);
						if(!p)
							p = "(null)";
						break;

					default:
						continue;
					}
					// If we didn't hit a continue, print out what we have in the buffer.
					if(length > 0)
					{
						while(length > 0)
						{
							Write(*p++);
							length--;
						}
					}
					else
					{
						while(*p)
							Write(*p++);
					}
				}
			}
		}

		void WriteFormat(const char *format, ...)
		{
			va_list arg;
			lock.Enter();
			va_start(arg, format);
			writef(format, &arg);
			va_end(arg);
			lock.Exit();
		}

		void WriteMessage(Style style, const char *message, const char *result, ...)
		{
			va_list arg;

			lock.Enter();
			va_start(arg, result);
			SetStyle(Style::TEXT);
			writef(message, &arg);
			Write('\t');
			SetStyle(style);
			writef(result, &arg);
			Write('\n');
			va_end(arg);
			lock.Exit();
		}
	}
}
