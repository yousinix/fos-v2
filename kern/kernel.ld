/* Simple linker script for the FOS kernel.
   See the GNU ld 'info' manual ("info ld") to learn the syntax. */

OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(start_of_kernel)

SECTIONS
{
	/* Load the kernel at this address: "." means the current address */
	. = 0xF0100000;

	.text : {
		*(.text .stub .text.* .gnu.linkonce.t.*)
	}

	PROVIDE(end_of_kernel_code_section = .);	/* Define the 'etext' symbol to this value */

	.rodata : {
		*(.rodata .rodata.* .gnu.linkonce.r.*)
	}

	/* Include debugging information in kernel memory */
	.stab : {
		PROVIDE(__STAB_BEGIN__ = .);
		*(.stab);
		PROVIDE(__STAB_END__ = .);
		BYTE(0)		/* Force the linker to allocate space
				   for this section */
	}

	.stabstr : {
		PROVIDE(__STABSTR_BEGIN__ = .);
		*(.stabstr);
		PROVIDE(__STABSTR_END__ = .);
		BYTE(0)		/* Force the linker to allocate space
				   for this section */
	}

	/* Adjust the address for the data segment to the next page */
	. = ALIGN(0x1000);

	/* The data segment */
	.data : {
		*(.data)
	}

	PROVIDE(start_of_uninitialized_data_section = .);

	.bss : {
		*(.bss)
	}

	PROVIDE(end_of_kernel = .);

	/DISCARD/ : {
		*(.eh_frame .note.GNU-stack)
	}
}
