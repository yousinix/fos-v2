#ifndef FOS_INC_ELF_H
#define FOS_INC_ELF_H

#define ELF_MAGIC 0x464C457FU	/* "\x7FELF" in little endian */

struct Elf {
	uint32 e_magic;	// must equal ELF_MAGIC
	uint8 e_elf[12];
	uint16 e_type;
	uint16 e_machine;
	uint32 e_version;
	uint32 e_entry;
	uint32 e_phoff;
	uint32 e_shoff;
	uint32 e_flags;
	uint16 e_ehsize;
	uint16 e_phentsize;
	uint16 e_phnum;
	uint16 e_shentsize;
	uint16 e_shnum;
	uint16 e_shstrndx;
};

struct Proghdr {
	uint32 p_type;
	uint32 p_offset;
	uint32 p_va;
	uint32 p_pa;
	uint32 p_filesz;
	uint32 p_memsz;
	uint32 p_flags;
	uint32 p_align;
};

struct Secthdr {
	uint32 sh_name;
	uint32 sh_type;
	uint32 sh_flags;
	uint32 sh_addr;
	uint32 sh_offset;
	uint32 sh_size;
	uint32 sh_link;
	uint32 sh_info;
	uint32 sh_addralign;
	uint32 sh_entsize;
};

// Values for Proghdr::p_type
#define ELF_PROG_LOAD		1

// Flag bits for Proghdr::p_flags
#define ELF_PROG_FLAG_EXEC	1
#define ELF_PROG_FLAG_WRITE	2
#define ELF_PROG_FLAG_READ	4

// Values for Secthdr::sh_type
#define ELF_SHT_NULL		0
#define ELF_SHT_PROGBITS	1
#define ELF_SHT_SYMTAB		2
#define ELF_SHT_STRTAB		3

// Values for Secthdr::sh_name
#define ELF_SHN_UNDEF		0

#endif /* !FOS_INC_ELF_H */
