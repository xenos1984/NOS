// InputOutputStream.h - Base class for all input / output streams.

#ifndef __INPUTOUTPUTSTREAM_H__
#define __INPUTOUTPUTSTREAM_H__

#include <InputStream.h>
#include <OutputStream.h>

namespace Stdlib
{
	class InputOutputStream : public InputStream, public OutputStream
	{
	public:
		bool CanRead() { return true; };
		bool CanWrite() { return true; };
	};
}

#endif
