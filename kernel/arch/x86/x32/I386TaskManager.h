// I386TaskManager.h - Switching tasks in a multiprocessor environment.

#ifndef __ARCH_X86_X32_TASKMANAGER_H__
#define __ARCH_X86_X32_TASKMANAGER_H__

#include <cstdint>
#include <TaskManager.h>
#include INC_VENDOR(IOApic.h)
#include INC_ARCH(Processor.h)

namespace Kernel
{
	/**
	 * Architecture specific task manager class.
	 * This class is the actual task switcher for the i386 architecture. It supports the use of multiple CPUs.
	 */
	class I386TaskManager : public TaskManager
	{
	private:
		unsigned char numcpus, numbuses, numioapics, numints, numlints;
//		Processor* cpu; /**< Points to a table with all CPUs. */
//		Processor* bsp; /**< Bootstrap Processor. */

		I386TaskManager(unsigned int nc);

	public:
		static void Init(void); /**< Create task manager for single CPU mode. */
#ifdef CONFIG_ACPI
		static void InitAcpi(void); /**< Create task manager for ACPI mode. */
#endif
#ifdef CONFIG_SMP
		static void InitSmp(void); /**< Create task manager for SMP mode. */
#endif

//		Processor* GetProcessor(unsigned char n); /**< Get Processor object by index. */
		void SetTSS(void);
		int GetCurrentCPU(void);
		Process* CreateProcess(Elf* elf);
		Thread* CreateThread(Process* p, void* entry, void* stack);
		void DeleteThread(Thread* t);
		void DeleteProcess(Process* p);
		void SwitchTo(Thread* from, Thread* to);
	};
}

inline Kernel::I386TaskManager& i386taskman(void) { return(reinterpret_cast<Kernel::I386TaskManager&>(taskman_space)); }

#endif
