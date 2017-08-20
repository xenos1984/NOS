// I386TaskManager.h - Switching tasks in a multiprocessor environment.

#ifndef __ARCH_X86_IA32_TASKMANAGER_H__
#define __ARCH_X86_IA32_TASKMANAGER_H__

#include <cstdint>
#include <TaskManager.h>
#include INC_VENDOR(IOApic.h)
#include INC_ARCH(Processor.h)

namespace Kernel
{
	/**
	 * i386 Task State Segment.
	 */
	struct TSS
	{
		uint16_t previous;
		uint16_t pad0;
		uint32_t esp0;
		uint16_t ss0;
		uint16_t pad1;
		uint32_t esp1;
		uint16_t ss1;
		uint16_t pad2;
		uint32_t esp2;
		uint16_t ss2;
		uint16_t pad3;
		uint32_t cr3;
		uint32_t eip;
		uint32_t eflags;
		uint32_t eax;
		uint32_t ecx;
		uint32_t edx;
		uint32_t ebx;
		uint32_t esp;
		uint32_t ebp;
		uint32_t esi;
		uint32_t edi;
		uint16_t es;
		uint16_t pad4;
		uint16_t cs;
		uint16_t pad5;
		uint16_t ss;
		uint16_t pad6;
		uint16_t ds;
		uint16_t pad7;
		uint16_t fs;
		uint16_t pad8;
		uint16_t gs;
		uint16_t pad9;
		uint16_t ldt;
		uint16_t pad10;
		uint16_t trap;
		uint16_t iobase;
	} PACKED;

	/**
	 * Architecture specific task manager class.
	 * This class is the actual task switcher for the i386 architecture. It supports the use of multiple CPUs.
	 */
	class I386TaskManager : public TaskManager
	{
	private:
		unsigned char numcpus, numbuses, numioapics, numints, numlints;
		Processor* cpu; /**< Points to a table with all CPUs. */
		Processor* bsp; /**< Bootstrap Processor. */

		I386TaskManager(unsigned int nc);

	public:
		static void Init(void); /**< Create task manager for single CPU mode. */
#ifdef CONFIG_ACPI
		static void InitAcpi(void); /**< Create task manager for ACPI mode. */
#endif
#ifdef CONFIG_SMP
		static void InitSmp(void); /**< Create task manager for SMP mode. */
#endif

		Processor* GetProcessor(unsigned char n); /**< Get Processor object by index. */
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
