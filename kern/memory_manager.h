/* See COPYRIGHT for copyright information. */

#ifndef FOS_KERN_MEM_MAN_H
#define FOS_KERN_MEM_MAN_H

#ifndef FOS_KERNEL
# error "This is a FOS kernel header; user programs should not #include it"
#endif

#include <inc/x86.h>
#include <inc/mmu.h>
#include <inc/error.h>
#include <inc/string.h>

#include <kern/helpers.h>
#include <kern/kclock.h>

#include <inc/memlayout.h>
#include <inc/assert.h>

#include <inc/environment_definitions.h>
#include "../inc/uheap.h"

#define TABLE_IN_MEMORY 0
#define TABLE_NOT_EXIST 1

//2015
uint32 _UHeapPlacementStrategy;

void setUHeapPlacementStrategyFIRSTFIT();
void setUHeapPlacementStrategyBESTFIT();
void setUHeapPlacementStrategyNEXTFIT();
void setUHeapPlacementStrategyWORSTFIT();

uint32 isUHeapPlacementStrategyFIRSTFIT();
uint32 isUHeapPlacementStrategyBESTFIT();
uint32 isUHeapPlacementStrategyNEXTFIT();
uint32 isUHeapPlacementStrategyWORSTFIT();

//***********************************
//2017
uint32 _KHeapPlacementStrategy;

void setKHeapPlacementStrategyCONTALLOC();
void setKHeapPlacementStrategyFIRSTFIT();
void setKHeapPlacementStrategyBESTFIT();
void setKHeapPlacementStrategyNEXTFIT();
void setKHeapPlacementStrategyWORSTFIT();

uint32 isKHeapPlacementStrategyCONTALLOC();
uint32 isKHeapPlacementStrategyFIRSTFIT();
uint32 isKHeapPlacementStrategyBESTFIT();
uint32 isKHeapPlacementStrategyNEXTFIT();
uint32 isKHeapPlacementStrategyWORSTFIT();

//***********************************
//2018 Memory Threshold
uint32 memory_scarce_threshold_percentage;	// Memory remains plentiful till the % of free frames gets below this threshold percentage
#define DEFAULT_MEM_SCARCE_PERCENTAGE 25	// Default threshold % of free memory to indicate scarce MEM

//Functions
uint32 calculate_free_frames();
//***********************************

struct freeFramesCounters
{
	int freeBuffered, freeNotBuffered, modified;
};

struct Env;

extern char ptr_stack_top[], ptr_stack_bottom[];

extern struct Frame_Info* frames_info;
extern struct Frame_Info* disk_frames_info;		// Virtual address of physical frames_info array
extern struct Linked_List free_frame_list;	// Free list of physical frames
extern struct Linked_List modified_frame_list;	// Free list of physical frames
extern uint32 number_of_frames;


extern uint32 phys_page_directory;
extern uint32 *ptr_page_directory;
extern uint8* ptr_zero_page;
extern uint8* ptr_temp_page;
extern char* ptr_free_mem;	// Pointer to next byte of free mem
extern char end_of_kernel[];

void boot_map_range(uint32 *ptr_page_directory, uint32 virtual_address, uint32 size, uint32 physical_address, int perm);
uint32* boot_get_page_table(uint32 *ptr_page_directory, uint32 virtual_address, int create);
void* boot_allocate_space(uint32 size, uint32 align);
void	initialize_kernel_VM();

void	initialize_paging();
int allocate_frame(struct Frame_Info **ptr_frame_info);
void free_frame(struct Frame_Info *ptr_frame_info);
int get_page_table(uint32 *ptr_page_directory, const void *virtual_address, uint32 **ptr_page_table);

//2016
void * create_page_table(uint32 *ptr_page_directory, const uint32 virtual_address);

int	map_frame(uint32 *ptr_page_directory, struct Frame_Info *ptr_frame_info, void *virtual_address, int perm);
void	unmap_frame(uint32 *pgdir, void *va);
struct Frame_Info *get_frame_info(uint32 *ptr_page_directory, void *virtual_address, uint32 **ptr_page_table);
void decrement_references(struct Frame_Info* ptr_frame_info);
void initialize_frame_info(struct Frame_Info *ptr_frame_info);

static inline uint32 to_frame_number(struct Frame_Info *ptr_frame_info)
{
	return ptr_frame_info - frames_info;
}

static inline uint32 to_physical_address(struct Frame_Info *ptr_frame_info)
{
	return to_frame_number(ptr_frame_info) << PGSHIFT;
}

static inline struct Frame_Info* to_frame_info(uint32 physical_address)
{
	if (PPN(physical_address) >= number_of_frames)
		panic("to_frame_info called with invalid pa");
	return &frames_info[PPN(physical_address)];
}


void freeMem(struct Env* e, uint32 virtual_address, uint32 size);
void allocateMem(struct Env* e, uint32 virtual_address, uint32 size);
void moveMem(struct Env* e, uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size);
uint32 calculate_required_frames(uint32* ptr_page_directory, uint32 start_virtual_address, uint32 size);
struct freeFramesCounters calculate_available_frames();
void trim_all_environments();


// WS helper functions ===================================================
inline uint32 env_page_ws_get_size(struct Env *e);
inline void env_page_ws_invalidate(struct Env* e, uint32 virtual_address);
inline void env_page_ws_set_entry(struct Env* e, uint32 entry_index, uint32 virtual_address);
inline void env_page_ws_clear_entry(struct Env* e, uint32 entry_index);
inline uint32 env_page_ws_get_virtual_address(struct Env* e, uint32 entry_index);
inline uint32 env_page_ws_is_entry_empty(struct Env* e, uint32 entry_index);
void env_page_ws_print(struct Env *curenv);


//page buffering functions
void bufferList_add_page(struct Linked_List* bufferList, struct Frame_Info *ptr_frame_info);
void bufferlist_remove_page(struct Linked_List* bufferList, struct Frame_Info *ptr_frame_info);


//Page tables entries
inline void pt_clear_page_table_entry(struct Env *e, uint32 virtual_address);
inline void pt_set_page_permissions(struct Env *e, uint32 virtual_address, uint32 permissions_to_set, uint32 permissions_to_clear);
inline uint32 pt_get_page_permissions(struct Env *e, uint32 virtual_address );

//2014
inline void add_frame_to_storage(uint32* frames_storage, struct Frame_Info* ptr_frame_info, uint32 index);
inline struct Frame_Info* get_frame_from_storage(uint32* frames_storage, uint32 index);
inline void clear_frames_storage(uint32* frames_storage);

//2016
#define CHECK_IF_KERNEL_ADDRESS(virtual_address) ( (uint32)virtual_address >= (uint32)USER_TOP && (uint32)virtual_address <= (uint32)0xFFFFFFFF)
#endif /* !FOS_KERN_MEM_MAN_H */
