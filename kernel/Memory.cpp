// Memory.cpp - General memory information and management.

#include <cstddef>

extern "C" void* memset(void* bufptr, int value, size_t size)
{
	unsigned char* buf = (unsigned char*)bufptr;

	for (size_t i = 0; i < size; i++)
		buf[i] = (unsigned char)value;

	return bufptr;
}

extern "C" void* memmove(void* dstptr, const void* srcptr, size_t size)
{
	unsigned char* dst = (unsigned char*)dstptr;
	const unsigned char* src = (const unsigned char*)srcptr;

	if (dst < src)
	{
		for (size_t i = 0; i < size; i++)
			dst[i] = src[i];
	}
	else
	{
		for (size_t i = size; i != 0; i--)
			dst[i - 1] = src[i - 1];
	}

	return dstptr;
}

extern "C" void* memcpy(void* __restrict dstptr, const void* __restrict srcptr, size_t size)
{
	unsigned char* dst = (unsigned char*)dstptr;
	const unsigned char* src = (const unsigned char*)srcptr;

	for (size_t i = 0; i < size; i++)
		dst[i] = src[i];

	return dstptr;
}

extern "C" int memcmp(const void* aptr, const void* bptr, size_t size)
{
	const unsigned char* a = (const unsigned char*) aptr;
	const unsigned char* b = (const unsigned char*) bptr;

	for (size_t i = 0; i < size; i++)
	{
		if (a[i] < b[i])
			return -1;
		else if (b[i] < a[i])
			return 1;
	}

	return 0;
}
