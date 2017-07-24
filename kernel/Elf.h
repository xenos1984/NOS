// Elf.h - Elf file structures.

#ifndef __ELF_H__
#define __ELF_H__

#include <cstdint>

namespace Kernel
{
	/** Generic ELF file. */
	class Elf
	{
	private:
		typedef uint32_t Addr32;
		typedef uint16_t Half32;
		typedef int16_t SHalf32;
		typedef uint32_t Off32;
		typedef int32_t Sword32;
		typedef uint32_t Word32;

		typedef uint64_t Addr64;
		typedef uint16_t Half64;
		typedef int16_t SHalf64;
		typedef uint64_t Off64;
		typedef int32_t Sword64;
		typedef uint32_t Word64;
		typedef uint64_t Xword64;
		typedef int64_t Sxword64;

		/** 32 bit ELF file header. */
		struct Header32
		{
			uint8_t Magic[4];       /**< Identification bytes / magic. */
			uint8_t Class;          /**< ELF file class (capacity). */
			uint8_t Data;           /**< ELF data encoding. */
			uint8_t Version;        /**< ELF version. */
			uint8_t OSABI;          /**< OS ABI. */
			uint8_t ABIVersion;     /**< ABI version. */
			uint8_t Reserved[7];    /**< Padding bytes. */
			Half32 Type;        /**< ELF file type. */
			Half32 Machine;     /**< Machine type. */
			Word32 Version2;    /**< ELF version (again). */
			Addr32 Entry;       /**< Program entry point, if any. */
			Off32 PHOffset;     /**< Offset of program header table. */
			Off32 SHOffset;     /**< Offset of section header table. */
			Word32 Flags;       /**< Processor specific flags. */
			Half32 EHSize;      /**< Size of the ELF section header, in bytes. */
			Half32 PHEntrySize; /**< Size of a program header table entry, in bytes. */
			Half32 PHNum;       /**< Number of program header table entries. */
			Half32 SHEntrySize; /**< Size of a section header table entry, in bytes. */
			Half32 SHNum;       /**< Number of section header table entries. */
			Half32 Shndx;       /**< Section header table index associated with the section name string table. */
		} PACKED;

		/** 64 bit ELF file header. */
		struct Header64
		{
			uint8_t Magic[4];       /**< Identification bytes / magic. */
			uint8_t Class;          /**< ELF file class (capacity). */
			uint8_t Data;           /**< ELF data encoding. */
			uint8_t Version;        /**< ELF version. */
			uint8_t OSABI;          /**< OS ABI. */
			uint8_t ABIVersion;     /**< ABI version. */
			uint8_t Reserved[7];    /**< Padding bytes. */
			Half64 Type;        /**< ELF file type. */
			Half64 Machine;     /**< Machine type. */
			Word64 Version2;    /**< ELF version (again). */
			Addr64 Entry;       /**< Program entry point, if any. */
			Off64 PHOffset;     /**< Offset of program header table. */
			Off64 SHOffset;     /**< Offset of section header table. */
			Word64 Flags;       /**< Processor specific flags. */
			Half64 EHSize;      /**< Size of the ELF section header, in bytes. */
			Half64 PHEntrySize; /**< Size of a program header table entry, in bytes. */
			Half64 PHNum;       /**< Number of program header table entries. */
			Half64 SHEntrySize; /**< Size of a section header table entry, in bytes. */
			Half64 SHNum;       /**< Number of section header table entries. */
			Half64 Shndx;       /**< Section header table index associated with the section name string table. */
		} PACKED;

		/** 32 bit ELF section header. */
		struct SectionHeader32
		{
			Word32 Name;      /**< String table index referencing the section name. */
			Word32 Type;      /**< Section type. */
			Word32 Flags;     /**< Section attribute flags. */
			Addr32 Address;   /**< Expected section address in memory. */
			Off32 Offset;     /**< Offset of the section. */
			Word32 Size;      /**< Size of the section in bytes. */
			Word32 Link;      /**< Section header table index, interpretation depends on section type. */
			Word32 Info;      /**< Extra information, interpretation depends on section type. */
			Word32 Align;     /**< Address alignment constraint. */
			Word32 EntrySize; /**< Entry size (if this section holds a table of fixed-size entries). */
		} PACKED;

		/** 64 bit ELF section header. */
		struct SectionHeader64
		{
			Word64 Name;       /**< String table index referencing the section name. */
			Word64 Type;       /**< Section type. */
			Xword64 Flags;     /**< Section attribute flags. */
			Addr64 Address;    /**< Expected section address in memory. */
			Off64 Offset;      /**< Offset of the section. */
			Xword64 Size;      /**< Size of the section in bytes. */
			Word64 Link;       /**< Section header table index, interpretation depends on section type. */
			Word64 Info;       /**< Extra information, interpretation depends on section type. */
			Xword64 Align;     /**< Address alignment constraint. */
			Xword64 EntrySize; /**< Entry size (if this section holds a table of fixed-size entries). */
		} PACKED;

		/** 32 bit ELF program header. */
		struct ProgramHeader32
		{
			Word32 Type;        /**< Segment type. */
			Off32 Offset;       /**< Offset of this segment in the file. */
			Addr32 VirtAddress; /**< Virtual address of this segment. */
			Addr32 PhysAddress; /**< Physical address of this segment. */
			Word32 FileSize;    /**< Size of this segment in the file image, in bytes. */
			Word32 MemSize;     /**< Size of this segment in memory, in bytes. */
			Word32 Flags;       /**< Segment flags. */
			Word32 Align;       /**< Section alignment constraints. */
		} PACKED;

		/** 64 bit ELF program header. */
		struct ProgramHeader64
		{
			Word64 Type;        /**< Segment type. */
			Word64 Flags;       /**< Segment flags. */
			Off64 Offset;       /**< Offset of this segment in the file. */
			Addr64 VirtAddress; /**< Virtual address of this segment. */
			Addr64 PhysAddress; /**< Physical address of this segment. */
			Xword64 FileSize;   /**< Size of this segment in the file image, in bytes. */
			Xword64 MemSize;    /**< Size of this segment in memory, in bytes. */
			Xword64 Align;      /**< Section alignment constraints. */
		} PACKED;

		/** 32 bit ELF symbol table entry. */
		struct Symbol32
		{
			Word32 Name;  /**< String table index referencing the symbol name. */
			Addr32 Value; /**< Symbol value. */
			Word32 Size;  /**< Associated size, if any. */
			uint8_t Info;     /**< Symbol type and binding. */
			uint8_t Other;    /**< Unused. */
			Half32 Shndx; /**< Section associated with this symbol. */
		} PACKED;

		/** 64 bit ELF symbol table entry. */
		struct Symbol64
		{
			Word64 Name;  /**< String table index referencing the symbol name. */
			uint8_t Info;     /**< Symbol type and binding. */
			uint8_t Other;    /**< Unused. */
			Half64 Shndx; /**< Section associated with this symbol. */
			Addr64 Value; /**< Symbol value. */
			Xword64 Size; /**< Associated size, if any. */
		} PACKED;

		/** 32 bit ELF relocation table entry. */
		struct Relocation32
		{
			Addr32 Offset; /**< Location at which to apply the relocation action. */
			Word32 Info;   /**< Symbol table index and type of relocation. */
		} PACKED;

		/** 64 bit ELF relocation table entry. */
		struct Relocation64
		{
			Addr64 Offset; /**< Location at which to apply the relocation action. */
			Word64 Info;   /**< Symbol table index and type of relocation. */
		} PACKED;

		/** 32 bit ELF relocation table entry with addend. */
		struct RelocationA32
		{
			Addr32 Offset;  /**< Location at which to apply the relocation action. */
			Word32 Info;    /**< Symbol table index and type of relocation. */
			Sword32 Addend; /**< Explicit addend. */
		} PACKED;

		/** 64 bit ELF relocation table entry with addend. */
		struct RelocationA64
		{
			Addr64 Offset;   /**< Location at which to apply the relocation action. */
			Xword64 Info;    /**< Symbol table index and type of relocation. */
			Sxword64 Addend; /**< Explicit addend. */
		} PACKED;

		/** 32 bit ELF dynamic table entry. */
		struct Dynamic32
		{
			Sword32 Tag; /**< Entry type. */
			union
			{
				Word32 Value;
				Addr32 Pointer;
			} Union;         /**< Value. */
		} PACKED;

		/** 64 bit ELF dynamic table entry. */
		struct Dynamic64
		{
			Sxword64 Tag; /**< Entry type. */
			union
			{
				Xword64 Value;
				Addr64 Pointer;
			} Union;          /**< Value. */
		} PACKED;

	public:
#if defined ELF32
		typedef Header32 Header;
		typedef ProgramHeader32 ProgramHeader;
		typedef SectionHeader32 SectionHeader;
		typedef Symbol32 Symbol;
		typedef Relocation32 Relocation;
		typedef RelocationA32 RelocationA;
		typedef Dynamic32 Dynamic;
		typedef Off32 Offset;
		typedef Addr32 Address;
#elif defined ELF64
		typedef Header64 Header;
		typedef ProgramHeader64 ProgramHeader;
		typedef SectionHeader64 SectionHeader;
		typedef Symbol64 Symbol;
		typedef Relocation64 Relocation;
		typedef RelocationA64 RelocationA;
		typedef Dynamic64 Dynamic;
		typedef Off64 Offset;
		typedef Addr64 Address;
#else
#error "Invalid word size for ELF format."
#endif

	private:
		Header header;

	public:
		// ELF Magic
		enum Magic : uint8_t
		{
			ELF_MAG0 = 0x7f,
			ELF_MAG1 = 'E',
			ELF_MAG2 = 'L',
			ELF_MAG3 = 'F'
		};

		// ELF file class (capacity)
		enum FileClass : uint8_t
		{
			ELF_CLASS_NONE = 0x00, /**< Invalid class. */
			ELF_CLASS_32   = 0x01, /**< 32Bit objects. */
			ELF_CLASS_64   = 0x02  /**< 64Bit objects. */
		};

		// ELF data encoding
		enum DataEncoding : uint8_t
		{
			ELF_DATA_NONE = 0x00, /**< Invalid data encoding. */
			ELF_DATA_2LSB = 0x01, /**< Least significant byte first. */
			ELF_DATA_2MSB = 0x02  /**< Most significant byte first. */
		};

		// ELF file type
		enum FileType : uint16_t
		{
			ET_NONE   = 0x0000, /**< No file type. */
			ET_REL    = 0x0001, /**< Relocatable file. */
			ET_EXEC   = 0x0002, /**< Executable file. */
			ET_DYN    = 0x0003, /**< Shared object file. */
			ET_CORE   = 0x0004, /**< Core file. */
			ET_LOPROC = 0xff00, /**< Processor specific. */
			ET_HIPROC = 0xffff  /**< Processor specific. */
		};

		// Machine type
		enum MachineType : uint16_t
		{
			EM_NONE        = 0x0000, /**< No machine. */
			EM_M32         = 0x0001, /**< AT&T WE 32100. */
			EM_SPARC       = 0x0002, /**< SPARC. */
			EM_386         = 0x0003, /**< Intel Architecture. */
			EM_68K         = 0x0004, /**< Motorola 68000. */
			EM_88K         = 0x0005, /**< Motorola 88000. */
			EM_860         = 0x0007, /**< Intel 80860. */
			EM_MIPS        = 0x0008, /**< MIPS RS3000 Big-Endian. */
			EM_MIPS_RS4_BE = 0x000a, /**< MIPS RS4000 Big-Endian. */
			EM_PPC         = 0x0014, /**< PowerPC. */
			EM_PPC64       = 0x0015, /**< PowerPC64. */
			EM_ARM         = 0x0028, /**< ARM. */
			EM_ALPHA       = 0x0029, /**< DEC Alpha. */
			EM_SPARCV9     = 0x002b, /**< SPARC V9. */
			EM_X86_64      = 0x003e, /**< AMD64. */
			EM_MBLAZE      = 0xbaab  /**< Xilinx MicroBlaze. */
		};

		// Special section indices
		enum SectionIndex : uint32_t
		{
			SHN_UNDEF     = 0x0000, /**< Undefined. */
			SHN_LORESERVE = 0xff00, /**< Reserved. */
			SHN_LOPROC    = 0xff00, /**< Processor specific. */
			SHN_HIPROC    = 0xff1f, /**< Processor specific. */
			SHN_ABS       = 0xfff1, /**< Absolute. */
			SHN_COMMON    = 0xfff2, /**< Common. */
			SHN_HIRESEVE  = 0xffff  /**< Reserved. */
		};

		// Section types
		enum SectionType : uint32_t
		{
			SHT_NULL     = 0x00000000, /**< No section. */
			SHT_PROGBITS = 0x00000001, /**< Program defined section (code, data...). */
			SHT_SYMTAB   = 0x00000002, /**< Symbol table. */
			SHT_STRTAB   = 0x00000003, /**< String table. */
			SHT_RELA     = 0x00000004, /**< Relocation table with explicit addends. */
			SHT_HASH     = 0x00000005, /**< Hash table. */
			SHT_DYNAMIC  = 0x00000006, /**< Information for dynamic linking. */
			SHT_NOTE     = 0x00000007, /**< Note. */
			SHT_NOBITS   = 0x00000008, /**< Empty, program defined section. */
			SHT_REL      = 0x00000009, /**< Relocation table. */
			SHT_SHLIB    = 0x0000000a, /**< Reserved. */
			SHT_DYNSYM   = 0x0000000b, /**< Dynamic symbol table. */
			SHT_LOPROC   = 0x70000000, /**< Processor specific. */
			SHT_HIPROC   = 0x7fffffff, /**< Processor specific. */
			SHT_LOUSER   = 0x80000000, /**< User defined. */
			SHT_HIUSER   = 0xffffffff  /**< User defined. */
		};

		// Section attribute files
		enum SectionAttribute : uint32_t
		{
			SHF_WRITE     = 0x00000001, /**< Section is writeable. */
			SHF_ALLOC     = 0x00000002, /**< Section occupies memory during process execution. */
			SHF_EXECINSTR = 0x00000004, /**< Section contains executable machine extructions. */
			SHF_MASKPROC  = 0xf0000000  /**< Reserved for processor specific semantics. */
		};

		// Symbol binding
		enum SymbolBinding : uint8_t
		{
			STB_LOCAL  = 0x0, /**< Local symbol. */
			STB_GLOBAL = 0x1, /**< Global symbol. */
			STB_WEAK   = 0x2, /**< Global symbol with lower precedence. */
			STB_LOPROC = 0xd, /**< Processor specific. */
			STB_HIPROC = 0xf  /**< Processor specific. */
		};

		// Symbol types
		enum SymbolType : uint8_t
		{
			STT_NOTYPE  = 0x0, /**< No type specified. */
			STT_OBJECT  = 0x1, /**< Data object. */
			STT_FUNC    = 0x2, /**< Function, executable code. */
			STT_SECTION = 0x3, /**< Section. */
			STT_FILE    = 0x4, /**< File symbol. */
			STT_LOPROC  = 0xd, /**< Processor specific. */
			STT_HIPROC  = 0xf  /**< Processor specific. */
		};

		// Segment types
		enum SegmentType : uint32_t
		{
			PT_NULL    = 0x00000000, /**< Unused. */
			PT_LOAD    = 0x00000001, /**< Loadable segment. */
			PT_DYNAMIC = 0x00000002, /**< Information for dynamic linking. */
			PT_INTERP  = 0x00000003, /**< Null-terminated path name to invoke as an interpreter. */
			PT_NOTE    = 0x00000004, /**< Auxiliary information. */
			PT_SHLIB   = 0x00000005, /**< Reserved. */
			PT_PHDR    = 0x00000006, /**< Program header. */
			PT_LOPROC  = 0x70000000, /**< Processor specific. */
			PT_HIPROC  = 0x7fffffff  /**< Processor specific. */
		};

		// Dynamic table entries
		enum DynamicEntry : uint32_t
		{
			DT_NULL         = 0x00000000, /**< Marks the end of the dynamic array. */
			DT_NEEDED       = 0x00000001, /**< The string table offset of the name of a needed library. */
			DT_PLTRELSZ     = 0x00000002, /**< Total size, in bytes, of the relocation entries associated with the procedure linkage table. */
			DT_PLTGOT       = 0x00000003, /**< Contains an address associated with the linkage table. */
			DT_HASH         = 0x00000004, /**< Address of the symbol hash table. */
			DT_STRTAB       = 0x00000005, /**< Address of the dynamic string table. */
			DT_SYMTAB       = 0x00000006, /**< Address of the dynamic symbol table. */
			DT_RELA         = 0x00000007, /**< Address of a relocation table with RelA entries. */
			DT_RELASZ       = 0x00000008, /**< Total size, in bytes, of the DT_RELA relocation table. */
			DT_RELAENT      = 0x00000009, /**< Size, in bytes, of each DT_RELA relocation entry. */
			DT_STRSZ        = 0x0000000a, /**< Total size, in bytes, of the string table. */
			DT_SYMENT       = 0x0000000b, /**< Size, in bytes, of each symbol table entry. */
			DT_INIT         = 0x0000000c, /**< Address of the initialization function. */
			DT_FINI         = 0x0000000d, /**< Address of the termination function. */
			DT_SONAME       = 0x0000000e, /**< The string table offset of the name of this shared object. */
			DT_RPATH        = 0x0000000f, /**< The string table offset of a shared library search path string. */
			DT_SYMBOLIC     = 0x00000010, /**< Modifies the symbol resolution algorithm for references within the library. */
			DT_REL          = 0x00000011, /**< Address of a relocation table with Rel64 entries. */
			DT_RELSZ        = 0x00000012, /**< Total size, in bytes, of the DT_REL relocation table. */
			DT_RELENT       = 0x00000013, /**< Size, in bytes, of each DT_REL relocation entry. */
			DT_PLTREL       = 0x00000014, /**< Type of relocation entry used for the procedure linkage table. */
			DT_DEBUG        = 0x00000015, /**< Reserved for debugger use. */
			DT_TEXTREL      = 0x00000016, /**< The relocation table contains relocations for a non-writable segment. */
			DT_JMPREL       = 0x00000017, /**< Address of the relocations associated with the procedure linkage table. */
			DT_BIND_NOW     = 0x00000018, /**< The dynamic loader should process all relocations for this object before transferring control to the program. */
			DT_INIT_ARRAY   = 0x00000019, /**< Pointer to an array of pointers to initialization functions. */
			DT_FINI_ARRAY   = 0x0000001a, /**< Pointer to an array of pointers to termination functions. */
			DT_INIT_ARRAYSZ = 0x0000001b, /**< Size, in bytes, of the array of initialization functions. */
			DT_FINI_ARRAYSZ = 0x0000001c  /**< Size, in bytes, of the array of termination functions. */
		};

		Elf(void);

		inline Header* GetHeader(void)
		{
			return &header;
		}

		inline ProgramHeader* GetProgramHeader(void)
		{
			return (ProgramHeader*)(header.PHOffset + (Address)&header);
		}

		inline SectionHeader* GetSectionHeader(void)
		{
			return (SectionHeader*)(header.SHOffset + (Address)&header);
		}
	};
}

extern Kernel::Elf::Header kernelElfHeader;
extern Kernel::Elf::Symbol kernelElfDynsym[];
extern char kernelElfDynstr[];
extern char kernelElfDynamic[];
extern char kernelElfHash[];

inline Kernel::Elf& kernelElf(void) { return(reinterpret_cast<Kernel::Elf&>(kernelElfHeader)); }

#endif
