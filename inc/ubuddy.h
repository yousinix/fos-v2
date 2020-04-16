#ifndef FOS_INC_UBUDDY_H
#define FOS_INC_UBUDDY_H 1

typedef LIST_ENTRY(BuddyNode) Buddy_LIST_entry_t;
LIST_HEAD(BuddyList, BuddyNode);		// Declares 'struct WS_list'

//TODO: [PROJECT 2020 - [0 GIVEN] Buddy Constants]
#define BUDDY_LOWER_LEVEL 1
#define BUDDY_UPPER_LEVEL 11
#define BUDDY_LIMIT 1<<BUDDY_UPPER_LEVEL		//BuddyLimit = 2^U
#define BUDDY_NUM_FREE_NODES 40000

//Values for BuddyNode Status
#define FREE 0		//in BuddyFreeNodesList
#define AVAILABLE 1		//in BuddyLevels
#define NONLEAF 2		//parent of 2 splitted nodes
#define ALLOCATED 3		//in BuddyAllocatedNodesList
//=====================================================================

//TODO: [PROJECT 2020 - [0 GIVEN] Buddy Node Struct & Lists]
struct BuddyNode
{
	Buddy_LIST_entry_t prev_next_info;	/* free list link */
	uint32 va;
	struct BuddyNode * parent;
	uint8 status;
	uint8 level;
	struct BuddyNode * myBuddy;
};

//List of available BuddyNode elements
struct BuddyList BuddyFreeNodesList ;
//Array of lists; Each list is a level that contains the available BuddyNode elements in it
struct BuddyList BuddyLevels[BUDDY_UPPER_LEVEL + 1] ;
//================================================================================

struct BuddyNode FreeNodes[BUDDY_NUM_FREE_NODES];

//TODO: [PROJECT 2020 - [0 GIVEN] Buddy Helper Functions]
#define BUDDY_NODE_SIZE(LEVEL) (1<<LEVEL)
void ClearNodeData(struct BuddyNode* node);
//=======================================================

void initialize_buddy() ;
void CreateNewBuddySpace();
void* FindAllocationUsingBuddy(int size);
void FreeAllocationUsingBuddy(uint32 va);

#endif
