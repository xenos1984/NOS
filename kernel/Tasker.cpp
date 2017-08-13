// Tasker.cpp - Task management.

#include <Tasker.h>
#include <Thread.h>
#include <Process.h>
#include <Elf.h>

namespace Kernel
{

	namespace Tasker
	{
		Process* CreateProcess(Elf* elf, const char* cmd)
		{
			// Create process image.
			Process* p = new Process(elf);

			// TODO: Enter process into list.

			// Create first thread that jumps to entry point.
			CreateThread(p, elf->GetHeader()->Entry);
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

