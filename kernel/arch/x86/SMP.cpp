// SMP.cpp - Symmetric multiprocessing management.

#include <new>
#include INC_ARCH(SMP.h)
#include INC_ARCH(Apic.h)
#include <Console.h>
#include <Symbol.h>

#ifdef CONFIG_SMP
namespace Kernel
{
	namespace SMP
	{
		int numcpus, numbuses, numioapics, numints, numlints;
		Cpu** cpus;
		Bus** buses;
		IOApic** ioapics;
		Interrupt** ints;
		Interrupt** lints;

		FloatingPointer* pointer;
		ConfigTable* table;

		static struct
		{
			ConfigTable table;
			Cpu cpu[2];
			Bus bus[2];
			IOApic ioapic;
			Interrupt intin[16];
			Interrupt lintin[2];
		} smpdefconfig SECTION(".init.data") =
		{
			{TABLE_MAGIC, 252, 0, 0,
				{'N', 'O', 'S', ' ', ' ', ' ', ' ', ' '},
				{'S', 'M', 'P', ' ', 'D', 'e', 'f', 'a', 'u', 'l', 't', 's'},
				0, 0, 23, 0xfee00000, 0, 0, 0},
			{
				{ENTRY_PROCESSOR, 0, 0, CPU_ENABLED | CPU_BSP, 0, 0, {0 , 0}},
				{ENTRY_PROCESSOR, 1, 0, CPU_ENABLED, 0, 0, {0 , 0}}
			},
			{
				{ENTRY_BUS, 0, {'I', 'S', 'A', ' ', ' ', ' '}},
				{255, 1, {'P', 'C', 'I', ' ', ' ', ' '}}
			},
			{ENTRY_IO_APIC, 0, 0, 1, 0xfec00000},
			{
				{ENTRY_IO_IRQ, 3, 0, 0, 0, 0xff, 0},
				{ENTRY_IO_IRQ, 0, 0, 0, 1, 0xff, 1},
				{ENTRY_IO_IRQ, 0, 0, 0, 2, 0xff, 2},
				{ENTRY_IO_IRQ, 0, 0, 0, 3, 0xff, 3},
				{ENTRY_IO_IRQ, 0, 0, 0, 4, 0xff, 4},
				{ENTRY_IO_IRQ, 0, 0, 0, 5, 0xff, 5},
				{ENTRY_IO_IRQ, 0, 0, 0, 6, 0xff, 6},
				{ENTRY_IO_IRQ, 0, 0, 0, 7, 0xff, 7},
				{ENTRY_IO_IRQ, 0, 0, 0, 8, 0xff, 8},
				{ENTRY_IO_IRQ, 0, 0, 0, 9, 0xff, 9},
				{ENTRY_IO_IRQ, 0, 0, 0, 10, 0xff, 10},
				{ENTRY_IO_IRQ, 0, 0, 0, 11, 0xff, 11},
				{ENTRY_IO_IRQ, 0, 0, 0, 12, 0xff, 12},
				{ENTRY_IO_IRQ, 0, 0, 0, 13, 0xff, 13},
				{ENTRY_IO_IRQ, 0, 0, 0, 14, 0xff, 14},
				{ENTRY_IO_IRQ, 0, 0, 0, 15, 0xff, 15}
			},
			{
				{ENTRY_LOCAL_IRQ, 3, 0, 0, 15, 0xff, 0},
				{ENTRY_LOCAL_IRQ, 0, 0, 0, 15, 0xff, 1}
			}
		};

		SECTION(".init.text") void Init(FloatingPointer* sfp)
		{
			unsigned char* ptr;
			unsigned int i;

			pointer = sfp;
			if(pointer->Table) // config table present?
			{
				Console::WriteMessage(Console::Style::OK, "SMP:", "individual config found at 0x%8x", pointer->Table);
				table = (ConfigTable*)(pointer->Table + Symbol::kernelOffset.Addr());
			}
			else // default config?
			{
				Console::WriteMessage(Console::Style::OK, "SMP:", "default config %d", pointer->Feature[0]);
				if(pointer->Feature[0] > 4)
				{
					smpdefconfig.bus[1].Type = ENTRY_BUS;
					smpdefconfig.cpu[0].LocalApicVersion = 0x10;
					smpdefconfig.cpu[1].LocalApicVersion = 0x10;
				}
				switch(pointer->Feature[0])
				{
				case 1:
				case 5:
					break;
				case 2:
					smpdefconfig.intin[2].Type = 255;
					smpdefconfig.intin[13].Type = 255;
				case 3:
				case 6:
					smpdefconfig.bus[0].TypeString[0] = 'E';
					smpdefconfig.bus[0].TypeString[1] = 'I';
					smpdefconfig.bus[0].TypeString[2] = 'S';
					smpdefconfig.bus[0].TypeString[3] = 'A';
					break;
				case 7:
					smpdefconfig.intin[0].Type = 255;
				case 4:
					smpdefconfig.bus[0].TypeString[0] = 'M';
					smpdefconfig.bus[0].TypeString[1] = 'C';
					smpdefconfig.bus[0].TypeString[2] = 'A';
					break;
				default:
					break;
				}
				table = (ConfigTable*)&smpdefconfig;
			}

			Console::WriteMessage(Console::Style::INFO, "CPU mode:", "multiple CPUs");
			Console::WriteMessage(Console::Style::INFO, "SMP hardware:", "vendor: %8s, product: %12s", table->OemString, table->ProductString);
			numcpus = numbuses = numioapics = numints = numlints = 0;
			Apic::Init(table->LocalApic);

			ptr = (unsigned char*)(sizeof(ConfigTable) + (unsigned long)table);

			for(i = 0; i < table->EntryCount; i++)
			{
				switch(ptr[0])
				{
				case ENTRY_PROCESSOR:
					numcpus++;
					ptr += 12;
					break;
				case ENTRY_BUS:
					numbuses++;
					break;
				case ENTRY_IO_APIC:
					numioapics++;
					break;
				case ENTRY_IO_IRQ:
					numints++;
					break;
				case ENTRY_LOCAL_IRQ:
					numlints++;
					break;
				default:
					break;
				}
				ptr += 8;
			}

			cpus = new Cpu*[numcpus];
			buses = new Bus*[numbuses];
			ioapics = new IOApic*[numioapics];
			ints = new Interrupt*[numints];
			lints = new Interrupt*[numlints];

			numcpus = numbuses = numioapics = numints = numlints = 0;

			ptr = (unsigned char*)(sizeof(ConfigTable) + (unsigned long)table);
			for(i = 0; i < table->EntryCount; i++)
			{
				switch(ptr[0])
				{
				case ENTRY_PROCESSOR:
					cpus[numcpus] = (Cpu*)ptr;
					Console::WriteMessage(Console::Style::INFO, "CPU:", "#%d = 0x%8x", cpus[numcpus]->LocalApicID, cpus[numcpus]->Signature);
					numcpus++;
					ptr += 12;
					break;
				case ENTRY_BUS:
					buses[numbuses] = (Bus*)ptr;
					Console::WriteMessage(Console::Style::INFO, "Bus:", "#%d = %6s", buses[numbuses]->ID, buses[numbuses]->TypeString);
					numbuses++;
					break;
				case ENTRY_IO_APIC:
					ioapics[numioapics] = (IOApic*)ptr;
					Console::WriteMessage(Console::Style::INFO, "IO APIC:", "#%d @ 0x%8x", ioapics[numioapics]->ApicID, ioapics[numioapics]->Address);
					numioapics++;
					break;
				case ENTRY_IO_IRQ:
					ints[numints] = (Interrupt*)ptr;
					Console::WriteMessage(Console::Style::INFO, "IO Interrupt:", "Bus #%d IRQ #%d => APIC #%d INT #%d", ints[numints]->SourceID, ints[numints]->SourceIRQ, ints[numints]->DestID, ints[numints]->DestIRQ);
					numints++;
					break;
				case ENTRY_LOCAL_IRQ:
					lints[numlints] = (Interrupt*)ptr;
					Console::WriteMessage(Console::Style::INFO, "Local Interrupt:", "Bus #%d IRQ #%d => APIC #%d INT #%d", lints[numlints]->SourceID, lints[numlints]->SourceIRQ, lints[numlints]->DestID, lints[numlints]->DestIRQ);
					numlints++;
					break;
				default:
					break;
				}
				ptr += 8;
			}
		}

		bool SECTION(".init.text") SearchPointer(Memory::PhysAddr first, Memory::PhysAddr last)
		{
			FloatingPointer* pointer;
			unsigned char checksum;
			unsigned char* csptr;
			unsigned long phys;
			int i;

			for(phys = first; phys < last; phys += 16) // 16 byte aligned
			{
				pointer = (FloatingPointer*)(phys + Symbol::kernelOffset.Addr());
				if(pointer->Magic == POINTER_MAGIC) // valid floating pointer
				{
					checksum = 0;
					csptr = (unsigned char*)pointer;
					for(i = 0; i < (pointer->Length << 4); i++)
						checksum += csptr[i];
					if((pointer->Table && pointer->Feature[0]) || (!pointer->Table && !pointer->Feature[0]) || (checksum > 0))
					{
						Console::WriteMessage(Console::Style::WARNING, "SMP floating pointer:", "invalid found at 0x%8x, ignored", phys);
					}
					else
					{
						Console::WriteMessage(Console::Style::OK, "SMP floating pointer:", "found at 0x%8x", phys);
						Init(pointer);
						return(true); // floating pointer found
					}
				}
			}

			Console::WriteMessage(Console::Style::INFO, "SMP floating pointer:", "not found in range 0x%8x to 0x%8x", first, last);
			return(false); // floating pointer not found
		}
		int SECTION(".init.text") GetISAIRQ(int n)
		{
			int i, j;
			for(i = 0; i < numbuses; i++)
			{
				if((buses[i]->TypeString[0] == 'I') && (buses[i]->TypeString[1] == 'S') && (buses[i]->TypeString[2] == 'A') && (buses[i]->TypeString[3] == ' ') && (buses[i]->TypeString[4] == ' ') && (buses[i]->TypeString[5] == ' '))
				{
					for(j = 0; j < numints; j++)
					{
						if((ints[j]->SourceID == buses[i]->ID) && (ints[j]->SourceIRQ == n))
							return(ints[j]->DestIRQ);
					}
					return(n);
				}
			}
			return(n);
		}
	}
}
#endif
