// Tasker.cpp - Task management.

#include <Tasker.h>
#include <Pager.h>
#include <Thread.h>
#include <Process.h>
#include <Elf.h>
#include <Processor.h>

namespace Kernel
{

	namespace Tasker
	{
		unsigned int numcpus;
		Processor* cpus;

		void Init(unsigned int nc)
		{
		}

		Process* CreateProcess(Elf* elf, const char* cmd)
		{
			if(!elf->Check())
				return nullptr;

			Elf::Header* fh = elf->GetHeader();

			if(fh->Type != Elf::ET_EXEC)
				return nullptr;

			Elf::ProgramHeader* ph = elf->GetProgramHeader();
			Memory::MemType mt;

			// TODO: Create new context and switch to it.

			// Go through the program header table.
			for(int i = 0; i < fh->PHNum; i++)
			{
				// Only do something if segment is loadable.
				if(ph[i].Type != Elf::PT_LOAD)
					continue;

				// Determine memory type to use from segment flags.
				switch(ph[i].Flags)
				{
				case Elf::PF_X:
				case Elf::PF_X | Elf::PF_R:
					mt = Memory::MemType::USER_EXEC;
					break;
				case Elf::PF_W:
				case Elf::PF_W | Elf::PF_R:
					mt = Memory::MemType::USER_RW;
					break;
				case Elf::PF_R:
					mt = Memory::MemType::USER_RO;
					break;
				default:
					return nullptr;
				}

				// TODO: Allocate memory.
				// TODO: Copy file contents.
			}

			ProcessData pd;

			// Create process image.
			Process* p = new Process(pd);

			// TODO: Enter process into list.

			// Create first thread that jumps to entry point.
			CreateThread(p, elf->GetHeader()->Entry);

			return p;
		}

		Thread* CreateThread(Process* p, uintptr_t entry)
		{
			// TODO: Create thread.
		}

		void DeleteThread(Thread* t)
		{
		}

		void DeleteProcess(Process* p)
		{
		}
	}
}
