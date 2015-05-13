// Thread.cpp - Threads for m68k architecture.

#include INC_ARCH(Thread.h)
#include <Console.h>

using namespace Kernel;

void ThreadData::Dump(void)
{
	console().WriteFormat("D0 = 0x%8x, A0 = 0x%8x\n", d0, a0);
	console().WriteFormat("D1 = 0x%8x, A1 = 0x%8x\n", d1, a1);
	console().WriteFormat("D2 = 0x%8x, A2 = 0x%8x\n", d2, a2);
	console().WriteFormat("D3 = 0x%8x, A3 = 0x%8x\n", d3, a3);
	console().WriteFormat("D4 = 0x%8x, A4 = 0x%8x\n", d4, a4);
	console().WriteFormat("D5 = 0x%8x, A5 = 0x%8x\n", d5, a5);
	console().WriteFormat("D6 = 0x%8x, A6 = 0x%8x\n", d6, a6);
	console().WriteFormat("D7 = 0x%8x, A7 = 0x%8x\n", d7, a7);
	console().WriteFormat("SR = 0x%4x, PC = 0x%8x\n", sr, pc);
}
