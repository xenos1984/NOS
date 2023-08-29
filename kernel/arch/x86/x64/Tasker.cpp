// Tasker.cpp - Task management.

#include INC_BITS(Tasker.h)
#include INC_ARCH(ControlRegisters.h)
#include INC_ARCH(MSR.h)

namespace Kernel
{

	namespace Tasker
	{
		void SwitchTo(Thread* from, Thread* to)
		{
			unsigned long cr0;

			// If the FPU / SSE has been used, save its contents.
			if((from != nullptr) && (from->data.mmstate != nullptr))
			{
				cr0 = CR0::Read();
				if((cr0 & CR0::TASK_SWITCHED) == 0)
					from->data.mmstate->Save();
				CR0::Write(cr0 | CR0::TASK_SWITCHED);
			}

			// Change kernel gs base and CR3 if necessary.
			if(to == nullptr)
			{
				MSR::Write(MSR::KERNEL_GS_BASE, 0);
				if(from != nullptr)
					CR3::Write(kprocess().data.cr3);
			}
			else
			{
				MSR::Write(MSR::KERNEL_GS_BASE, (uint64_t)&(to->data));
				if((from == nullptr) || (from->owner != to->owner))
					CR3::Write(to->owner->data.cr3);
			}
		}
	}
}
