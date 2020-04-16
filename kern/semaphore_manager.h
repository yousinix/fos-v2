#ifndef FOS_SEMAPHORE_MANAGER_H
#define FOS_SEMAPHORE_MANAGER_H
#ifndef FOS_KERNEL
# error "This is a FOS kernel header; user programs should not #include it"
#endif

#include <inc/types.h>
#include <inc/queue.h>
#include <inc/trap.h>
#include <inc/memlayout.h>
#include <inc/environment_definitions.h>
#include <kern/sched.h>



struct Semaphore
{
	//ID of the owner environment
	int32 ownerID ;

	//semaphore name
	char name[64];

	//queue of all blocked envs on this Semaphore
	struct Env_Queue env_queue;

	//semaphore value
	int value;

	//indicate whether this object is empty or used
	uint8 empty;
};

// Array of all Semaphores
#if USE_KHEAP == 0
	//max number of semaphores
	#define MAX_SEMAPHORES 100
	struct Semaphore semaphores[MAX_SEMAPHORES] ;
#else
	uint32 MAX_SEMAPHORES;
	struct Semaphore *semaphores;
#endif

int createSemaphore(int ownerEnvID, char* semaphoreName, uint32 initialValue);
void waitSemaphore(int ownerEnvID, char* semaphoreName);
void signalSemaphore(int ownerEnvID, char* semaphoreName);

//2017
void create_semaphores_array(uint32 numOfSemaphores);
int get_semaphore_object_ID(int32 ownerID, char* name);
int allocate_semaphore_object(struct Semaphore **allocatedObject);
int free_semaphore_object(uint32 semaphoreObjectID);

#endif /* FOS_SEMAPHORE_MANAGER_H */
