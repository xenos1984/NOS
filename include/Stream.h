// Stream.h - Base class for all streams.

#ifndef __STREAM_H__
#define __STREAM_H__

namespace Stdlib
{
	enum SeekOrigin
	{
		Begin,
		Current,
		End
	};

	class Stream
	{
	public:
		virtual bool CanRead() = 0;
		virtual bool CanWrite() = 0;
		virtual bool CanSeek() = 0;

		virtual void SetLength(long value) = 0;
		virtual long GetLength() = 0;

		virtual void SetPosition(long value) = 0;
		virtual long GetPosition() = 0;

		virtual void Close() = 0;

		virtual void Seek(long offset, SeekOrigin origin) = 0;

		virtual void Flush() = 0;
	};
}

#endif
