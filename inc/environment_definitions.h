/* See COPYRIGHT for copyright information. */

#ifndef FOS_INC_ENV_H
#define FOS_INC_ENV_H

#include <inc/types.h>
#include <inc/queue.h>
#include <inc/trap.h>
#include <inc/memlayout.h>

// An environment ID 'envid_t' has three parts:
//
// +1+---------------21-----------------+--------10--------+
// |0|          Uniqueifier             |   Environment    |
// | |                                  |      Index       |
// +------------------------------------+------------------+
//                                       \--- ENVX(eid) --/
//
// The environment index ENVX(eid) equals the environment's offset in the
// 'envs[]' array.  The uniqueifier distinguishes environments that were
// created at different times, but share the same environment index.
//
// All real environments are greater than 0 (so the sign bit is zero).
// envid_ts less than 0 signify errors.


//Sizes of working sets & LRU 2nd list [if NO KERNEL HEAP]
#define __TWS_MAX_SIZE 	50

//TODO: [PROJECT 2020 - [0 GIVEN] Size of Working Set & LRU Second List]
#define __PWS_MAX_SIZE 	5000
#define __LRU_SNDLST_SIZE 0

//2017: moved to shared_memory_manager
//#define MAX_SHARES 100
//====================================
unsigned int _ModifiedBufferLength;

//2017: Max length of program name
#define PROGNAMELEN 64

//2018 Percentage of the pages to be removed from the WS [either for scarce RAM or Full WS]
#define DEFAULT_PERCENT_OF_PAGE_WS_TO_REMOVE	10	// 10% of the loaded pages is required to be removed

// Values of env_status in struct Env
#define ENV_FREE		0
#define ENV_READY		1
#define ENV_RUNNABLE	2
#define ENV_BLOCKED		3
#define ENV_NEW			4
#define ENV_EXIT		5
#define ENV_UNKNOWN		6


uint32 old_pf_counter;
//uint32 mydblchk;
struct WorkingSetElement {
	unsigned int virtual_address;
	uint8 empty;

	//2020
	LIST_ENTRY(WorkingSetElement) prev_next_info;	// list link pointers
};

struct SharingVarInfo
{
	 uint32 start_va;
	 uint32 size;
	 uint32 owner_flag;
	 uint32 id_in_shares_array;
};

//2020
LIST_HEAD(WS_List, WorkingSetElement);		// Declares 'struct WS_list'
//======================================================================

struct Env {
	struct Trapframe env_tf;	// Saved registers
	LIST_ENTRY(Env) prev_next_info;	// Free list link pointers
	int32 env_id;			// Unique environment identifier
	int32 env_parent_id;		// env_id of this env's parent
	unsigned env_status;		// Status of the environment
	uint32 env_runs;		// Number of times environment has run

	// Address space
	uint32 *env_page_directory;		// Kernel virtual address of page dir
	uint32 env_cr3;		// Physical address of page dir

	//for page file management
	uint32* disk_env_pgdir;
	//2016
	unsigned int disk_env_pgdir_PA;

	//for table file management
	uint32* disk_env_tabledir;
	//2016
	unsigned int disk_env_tabledir_PA;

	//page working set management
	unsigned int page_WS_max_size;

#if USE_KHEAP == 0
	struct WorkingSetElement ptr_pageWorkingSet[__PWS_MAX_SIZE];
#else
	struct WorkingSetElement* ptr_pageWorkingSet;
#endif

	//table working set management
	struct WorkingSetElement __ptr_tws[__TWS_MAX_SIZE];

	uint32 page_last_WS_index;
	uint32 table_last_WS_index;

	uint32 pageFaultsCounter;
	uint32 tableFaultsCounter;

	uint32 freeingFullWSCounter;
	uint32 freeingScarceMemCounter;

	uint32 nModifiedPages;
	uint32 nNotModifiedPages;

	//2020
	uint32 nPageIn, nPageOut;

	//Program name (to print it via USER.cprintf in multitasking)
	char prog_name[PROGNAMELEN];

	//2016
	struct WorkingSetElement* __uptr_pws;

	//2018:
	//Percentage of WS pages to be removed [either for scarce RAM or Full WS]
	unsigned int percentage_of_WS_pages_to_be_removed;
	uint32 nClocks ;

	int priority;

	//2020
	//TODO: [PROJECT 2020 - [0 GIVEN] LRU Data Structures]
	struct WS_List PageWorkingSetList ;	//LRU Approx: List of available WS elements
	struct WS_List ActiveList ;		//LRU Approx: ActiveList that should work as FCFS
	struct WS_List SecondList ;		//LRU Approx: SecondList that should work as LRU
	int ActiveListSize ;			//LRU Approx: Max allowed size of ActiveList
	int SecondListSize ;			//LRU Approx: Max allowed size of SecondList
	//================================================================================
};

#define PRIORITY_LOW    		1
#define PRIORITY_BELOWNORMAL    2
#define PRIORITY_NORMAL		    3
#define PRIORITY_ABOVENORMAL    4
#define PRIORITY_HIGH		    5

#define LOG2NENV		10
//#define NENV			(1 << LOG2NENV)
#define NENV			( (PTSIZE/4) / sizeof(struct Env) )
#define ENVX(envid)		((envid) & (NENV - 1))

#endif // !FOS_INC_ENV_H
