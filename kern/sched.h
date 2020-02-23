/* See COPYRIGHT for copyright information. */

#ifndef FOS_KERN_SCHED_H
#define FOS_KERN_SCHED_H
#ifndef FOS_KERNEL
# error "This is a FOS kernel header; user programs should not #include it"
#endif

// This function does not return.
void sched_yield(void) __attribute__((noreturn));

#endif	// !FOS_KERN_SCHED_H
