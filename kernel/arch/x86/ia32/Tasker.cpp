// Tasker.cpp - Task management.

#include INC_SUBARCH(Tasker.h)
#include INC_ARCH(ControlRegisters.h)

namespace Kernel
{

	namespace Tasker
	{
		void SwitchTo(Thread* from, Thread* to)
		{
			unsigned long cr0;
			ThreadData** stacktop = (ThreadData**)(KernelStackAddr + (GetCurrentCPU() + 1) * KernelStackSize - 4);

			// If the FPU / SSE has been used, save its contents.
			if((from != nullptr) && (from->data.mmstate != nullptr))
			{
				cr0 = CR0::Read();
				if((cr0 & CR0::TASK_SWITCHED) == 0)
					from->data.mmstate->Save();
				CR0::Write(cr0 | CR0::TASK_SWITCHED);
			}

			// Change pointer to register save area and CR3 if necessary.
			if(to == nullptr)
			{
				*stacktop = nullptr;
				if(from != nullptr)
					CR3::Write(kprocess().data.cr3);
			}
			else
			{
				*stacktop = &(to->data);
				if((from == nullptr) || (from->owner != to->owner))
					CR3::Write(to->owner->data.cr3);
			}
		}
	}
}
