// Elf.cpp - Elf file structures.

#include <Elf.h>

namespace Kernel
{
	bool Elf::Check(void)
	{
		if(header.Magic[0] != ELF_MAG0 || header.Magic[1] != ELF_MAG1 || header.Magic[2] != ELF_MAG2 || header.Magic[3] != ELF_MAG3)
			return false;
#if defined ELF32
		if(header.Class != ELF_CLASS_32)
			return false;
#elif defined ELF64
		if(header.Class != ELF_CLASS_64)
			return false;
#endif
		if(header.Machine != kernelElfHeader.Machine)
			return false;

		if(!header.PHNum)
			return false;

		if(!header.SHNum)
			return false;

		return true;
	}
}

