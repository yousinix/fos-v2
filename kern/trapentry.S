/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>



###################################################################
# exceptions/interrupts
###################################################################

/* The TRAPHANDLER macro defines a globally-visible function for handling
 * a trap.  It pushes a trap number onto the stack, then jumps to _alltraps.
 * Use TRAPHANDLER for traps where the CPU automatically pushes an error code.
 */ 
#define TRAPHANDLER(name, num)						\
	.globl name;		/* define global symbol for 'name' */	\
	.type name, @function;	/* symbol type is function */		\
	.align 2;		/* align function definition */		\
	name:			/* function starts here */		\
	pushl $(num); \
	jmp _alltraps

/* Use TRAPHANDLER_NOEC for traps where the CPU doesn't push an error code.
 * It pushes a 0 in place of the error code, so the trap frame has the same
 * format in either case.
 */
#define TRAPHANDLER_NOEC(name, num)					\
	.globl name;							\
	.type name, @function;						\
	.align 2;							\
	name:								\
	pushl $0;							\
	pushl $(num);							\
	jmp _alltraps

.text

/*
 * Lab 3: Your code here for generating entry points for the different traps.
 */

TRAPHANDLER(PAGE_FAULT, T_PGFLT)		

TRAPHANDLER_NOEC(SYSCALL_HANDLER, T_SYSCALL)
	

/*
 * Lab 3: Your code here for _alltraps
 */
_alltraps:

push %ds 
push %es 
pushal 	

mov $(GD_KD), %ax 
mov %ax,%ds
mov %ax,%es

push %esp

call trap

pop %ecx /* poping the pointer to the tf from the stack so that the stack top is at the values of the registers posuhed by pusha*/
popal 	
pop %es 
pop %ds    

/*skipping the trap_no and the error code so that the stack top is at the old eip value*/
add $(8),%esp

iret


