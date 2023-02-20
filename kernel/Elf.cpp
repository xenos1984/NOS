// Elf.cpp - Elf file structures.

#include <Elf.h>
#include <Console.h>
#include <Memory.h>

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

		ProgramHeader* ph = GetProgramHeader();

		for(int i = 0; i < header.PHNum; i++)
		{
			if(ph[i].Type == PT_LOAD)
			{
				if(ph[i].Align < Memory::MinPageSize)
				{
					Console::WriteMessage(Console::Style::WARNING, "ELF check:", "Segment %d alignment 0x%x is smaller than page size 0x%x.", i, ph[i].Align, Memory::MinPageSize);
					return false;
				}

				if(ph[i].VirtAddress & Memory::MinPageMask)
				{
					Console::WriteMessage(Console::Style::WARNING, "ELF check:", "Segment %d virtual address 0x%p is misaligned.", i, ph[i].VirtAddress);
					return false;
				}
/*
				if(ph[i].PhysAddress & Memory::MinPageMask)
				{
					Console::WriteMessage(Console::Style::WARNING, "ELF check:", "Segment %d physical address 0x%p is misaligned.", i, ph[i].PhysAddress);
					return false;
				}
*/
				if(ph[i].Offset & Memory::MinPageMask)
				{
					Console::WriteMessage(Console::Style::WARNING, "ELF check:", "Segment %d offset 0x%p is misaligned.", i, ph[i].Offset);
					return false;
				}

				if((ph[i].Flags & (PF_W | PF_X)) == (PF_W | PF_X))
				{
					Console::WriteMessage(Console::Style::WARNING, "ELF check:", "Segment %d is both executable and writeable.", i);
					return false;
				}
			}
		}

		return true;
	}
}
