#ifndef FOS_KERN_SYSCALL_H
#define FOS_KERN_SYSCALL_H
#ifndef FOS_KERNEL
# error "This is a FOS kernel header; user programs should not #include it"
#endif

#include <inc/syscall.h>

uint32 syscall(uint32 num, uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5);


#endif /* !FOS_KERN_SYSCALL_H */
