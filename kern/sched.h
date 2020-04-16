/* See COPYRIGHT for copyright information. */

#ifndef FOS_KERN_SCHED_H
#define FOS_KERN_SCHED_H
#ifndef FOS_KERNEL
# error "This is a FOS kernel header; user programs should not #include it"
#endif

#include <inc/environment_definitions.h>

//2018
#define SCH_RR 0
#define SCH_MLFQ 1
unsigned scheduler_method ;

LIST_HEAD(Env_Queue, Env);		// Declares 'struct Env_Queue'
LIST_HEAD(Env_list, Env);		// Declares 'struct Env_list'

///Scheduler Queues
//=================
struct Env_Queue env_new_queue;	// queue of all new envs
//2015:
struct Env_Queue env_exit_queue;	// queue of all exited envs
//2018:
//2020:
#if USE_KHEAP
	struct Env_Queue *env_ready_queues;	// Ready queue(s) for the MLFQ or RR
	uint8 *quantums ;					// Quantum(s) in ms for each level of the ready queue(s)
#else
	//RR ONLY
	struct Env_Queue env_ready_queues[1];	// Ready queue(s) for the RR
	uint8 quantums[1] ;					// Quantum in ms for RR
#endif
uint8 num_of_ready_queues ;			// Number of ready queue(s)
//===============

//2015
#define SCH_STOPPED 0
#define SCH_STARTED 1
unsigned scheduler_status ;

#define CLOCK_INTERVAL_IN_MS 10 //milliseconds

//2017
//#define CLOCK_INTERVAL_IN_CNTS TIMER_DIV((1000/CLOCK_INTERVAL_IN_MS))

//2012
// This function does not return.
void fos_scheduler(void) __attribute__((noreturn));

void sched_init();
void clock_interrupt_handler();
void sched_insert_ready(struct Env* env);
void sched_remove_ready(struct Env* env);
void sched_insert_new(struct Env* env);
void sched_remove_new(struct Env* env);
void sched_print_all();
void sched_run_all();
void sched_run_env(uint32 envId);

//2015:
void sched_new_env(struct Env* e);
//void sched_new_env(uint32 envId);
void sched_exit_env(uint32 envId);
void sched_insert_exit(struct Env* env);
void sched_remove_exit(struct Env* env);
void sched_kill_env(uint32 envId);
void sched_kill_all();

//2018:
//Declaration of helper functions to deal with the env queues
void init_queue(struct Env_Queue* queue);
int queue_size(struct Env_Queue* queue);
void enqueue(struct Env_Queue* queue, struct Env* env);
struct Env* dequeue(struct Env_Queue* queue);
struct Env* find_env_in_queue(struct Env_Queue* queue, uint32 envID);
void remove_from_queue(struct Env_Queue* queue, struct Env* e);
void sched_init_RR(uint8 quantum);
void sched_init_MLFQ(uint8 numOfLevels, uint8 *quantumOfEachLevel);
uint32 isSchedMethodMLFQ();
uint32 isSchedMethodRR();
void sched_exit_all_ready_envs();
#endif	// !FOS_KERN_SCHED_H
