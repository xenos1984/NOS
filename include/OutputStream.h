// OutputStream.h - Base class for all output streams.

#ifndef __OUTPUTSTREAM_H__
#define __OUTPUTSTREAM_H__

#include <Stream.h>

namespace Stdlib
{
	class OutputStream : public Stream
	{
	public:
		virtual OutputStream& operator<<(const char *string) = 0;
		virtual OutputStream& operator<<(char chr)     = 0;
		virtual OutputStream& operator<<(short number) = 0;
		virtual OutputStream& operator<<(int number)  = 0;

		virtual void WriteByte(unsigned char value)     = 0;
		virtual void WriteWord(unsigned short value)    = 0;
		virtual void WriteLongWord(unsigned int value) = 0;

		virtual void WriteChar(char value)    = 0;
		virtual void WriteShort(short value)  = 0;
		virtual void WriteInteger(int value) = 0;

		bool CanRead() { return false; };
		bool CanWrite() { return true; };
	};
}

#endif
