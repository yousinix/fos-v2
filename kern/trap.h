/* See COPYRIGHT for copyright information. */

#ifndef FOS_KERN_TRAP_H
#define FOS_KERN_TRAP_H
#ifndef FOS_KERNEL
# error "This is a FOS kernel header; user programs should not #include it"
#endif

#include <inc/trap.h>
#include <inc/mmu.h>

/* The kernel's interrupt descriptor table */
extern struct Gatedesc idt[];

void idt_init(void);
void print_regs(struct PushRegs *regs);
void print_trapframe(struct Trapframe *tf);
void page_fault_handler(struct Trapframe *);
void backtrace(struct Trapframe *);

#endif /* FOS_KERN_TRAP_H */
