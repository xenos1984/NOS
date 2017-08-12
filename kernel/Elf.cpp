// Elf.cpp - Elf file structures.

#include <Elf.h>
#include <Console.h>

namespace Kernel
{
	bool Elf::Check(void)
	{
		if(header.Magic[0] != ELF_MAG0 || header.Magic[1] != ELF_MAG1 || header.Magic[2] != ELF_MAG2 || header.Magic[3] != ELF_MAG3)
		{
			Console::WriteMessage(Console::Style::WARNING, "ELF check:", "Invalid ELF magic.");
			return false;
		}
#if defined ELF32
		if(header.Class != ELF_CLASS_32)
		{
			Console::WriteMessage(Console::Style::WARNING, "ELF check:", "No 32 bit module.");
			return false;
		}
#elif defined ELF64
		if(header.Class != ELF_CLASS_64)
		{
			Console::WriteMessage(Console::Style::WARNING, "ELF check:", "No 64 bit module.");
			return false;
		}
#endif
		if(header.Machine != kernelElfHeader.Machine)
		{
			Console::WriteMessage(Console::Style::WARNING, "ELF check:", "Incorrect architecture 0x%4x, expected 0x%4x.", header.Machine, kernelElfHeader.Machine);
			return false;
		}

		if(!header.PHNum)
		{
			Console::WriteMessage(Console::Style::WARNING, "ELF check:", "No program header found.");
			return false;
		}

		if(!header.SHNum)
		{
			Console::WriteMessage(Console::Style::WARNING, "ELF check:", "No section header found.");
			return false;
		}

		return true;
	}
}

