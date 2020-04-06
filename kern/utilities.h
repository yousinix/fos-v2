#ifndef FOS_KERN_UTILITIES_H
#define FOS_KERN_UTILITIES_H
#ifndef FOS_KERNEL
# error "This is a FOS kernel header; user programs should not #include it"
#endif

#include <inc/x86.h>
#include <inc/timerreg.h>

void scarce_memory();
uint32 calc_no_pages_tobe_removed_from_ready_exit_queues();

struct Env *__pe, *__ne ;
uint8 __pl, __nl, __chkstatus ;
void chksch(uint8 onoff);
void chk1();
void chk2(struct Env* __se);

#endif /* !FOS_KERN_UTILITIES_H */
