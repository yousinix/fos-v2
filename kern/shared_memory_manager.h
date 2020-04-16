#ifndef FOS_SHARED_MEMORY_MANAGER_H
#define FOS_SHARED_MEMORY_MANAGER_H
#ifndef FOS_KERNEL
# error "This is a FOS kernel header; user programs should not #include it"
#endif

//#include <inc/types.h>
//#include <inc/queue.h>
//#include <inc/trap.h>
//#include <inc/memlayout.h>
#include <inc/environment_definitions.h>


struct Share
{
	//ID of the owner environment
	int32 ownerID ;
	//share name
	char name[64];
	//share size
	int size;
	//references, number of envs looking at this shared mem object
	uint32 references;
	//sharing permissions (0: ReadOnly, 1:Writable)
	uint8 isWritable;

	//indicate whether this object is empty or used
	uint8 empty;
	//to store frames to be shared
#if USE_KHEAP == 0
	uint32 framesStorage[1024];
#else
	uint32 *framesStorage;
#endif

};

//array of all shared objects
#if USE_KHEAP == 0
	//max number of shared objects
	#define MAX_SHARES 100
	struct Share shares[MAX_SHARES] ;
#else
	uint32 MAX_SHARES ;
	struct Share *shares;
#endif

int createSharedObject(int32 ownerID, char* shareName, uint32 size, uint8 isWritable, void* virtual_address);
int getSizeOfSharedObject(int32 ownerID, char* shareName);
int getSharedObject(int32 ownerID, char* shareName, void* virtual_address);
int freeSharedObject(int32 sharedObjectID, void *startVA);

//2017
int get_share_object_ID(int32 ownerID, char* name);
void create_shares_array(uint32 numOfElements);
int allocate_share_object(struct Share **allocatedObject);
int free_share_object(uint32 sharedObjectID);

#endif /* FOS_SHARED_MEMORY_MANAGER_H */
