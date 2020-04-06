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

extern uint8 bypassInstrLength ;

uint32 _EnableModifiedBuffer ;
uint32 _EnableBuffering ;


uint32 _PageRepAlgoType;
#define PG_REP_LRU 0x1
#define PG_REP_CLOCK 0x2
#define PG_REP_FIFO 0x3
#define PG_REP_MODIFIEDCLOCK  0x4

void idt_init(void);
void print_regs(struct PushRegs *regs);
void print_trapframe(struct Trapframe *tf);
void fault_handler(struct Trapframe *);
void backtrace(struct Trapframe *);

void setPageReplacmentAlgorithmLRU();
void setPageReplacmentAlgorithmCLOCK();
void setPageReplacmentAlgorithmFIFO();
void setPageReplacmentAlgorithmModifiedCLOCK();

uint32 isPageReplacmentAlgorithmLRU();
uint32 isPageReplacmentAlgorithmCLOCK();
uint32 isPageReplacmentAlgorithmFIFO();
uint32 isPageReplacmentAlgorithmModifiedCLOCK();

void enableModifiedBuffer(uint32 enableIt);
uint32 isModifiedBufferEnabled();
void page_placement(struct Env * curenv, uint32 fault_va);
#endif /* FOS_KERN_TRAP_H */
