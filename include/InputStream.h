// InputStream.h - Base class for all input streams.

#ifndef __INPUTSTREAM_H__
#define __INPUTSTREAM_H__

#include <Stream.h>

namespace Stdlib
{
	class InputStream : public Stream
	{
	public:
		virtual InputStream& operator>>(char *string) = 0;
		virtual InputStream& operator>>(char chr)     = 0;

		virtual unsigned char ReadByte()     = 0;
		virtual unsigned short ReadWord()    = 0;
		virtual unsigned int ReadLongWord() = 0;

		virtual char ReadChar()    = 0;
		virtual short ReadShort()  = 0;
		virtual int ReadInteger() = 0;

		bool CanRead() { return true; };
		bool CanWrite() { return false; };
	};
}

#endif
