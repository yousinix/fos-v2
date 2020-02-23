
#include <inc/lib.h>

/*
 * Simple malloc
 *
 * The address space for the dynamic allocation is 
 * from "ptr_user_free_mem" to USER_TOP-1
 * Pages are allocated, used to fill successive
 * malloc requests.
 * On succeed, return void pointer to the allocated space
 * return NULL if
 *	the required allocation is outside the user heap area
 *	the required number of frames (pages and page tables) is not available
 */

// Hint: since malloc deal with memory, and since we now in the user mode,
// so you will need to use some functions that switch to the kernel to apply 
// certain operation using the kern/memory_manager functions and 
// then switch back to the user mode to continue execution  
// These functions are:
// [1] uint32 sys_calculate_free_frames()
//	calculate the number of free frames in the physical memory and return it 
// [2] uint32 sys_calculate_required_frames(uint32 start_virtual_address, uint32 size)
// 	Calculate the required number of frames (for pages and page tables) 
//	for mapping the virtual range [start_virtual_address, start_virtual_address + size]
//  	and return it
// [3] int sys_get_page(void *virtual_address, int perm)
//	allocate one frame in the physical memory and map it to "virtual_address" 
//	with the given "perm" permissions. 
//	if the page is already mapped , sys_get_page will do nothing
//	If allocation and mapping succeed, it return 0
//	else, it return -ve value

 
static uint8 *ptr_user_free_mem  = (uint8*) USER_HEAP_START;

void* malloc(uint32 size)
{	
	//PROJECT 2008: your code here
	//	

	panic("malloc is not implemented yet");
	
	// [1] Check if the required allocation is within the user heap area, 
	//	if not then return NULL.


	// [2] Calculate the required number of frames (for pages and page tables) for the new heap allocation. 
	//	Hint: Use function sys_calculate_required_frames(uint32 start_virtual_address,uint32 size).
	// 	This function will switch to the kernel mode, calls the kernel function 
	//	calculate_required_frames(uint32* ptr_page_directory, uint32 start_virtual_address, uint32 size) in 
	//	"memory_manager.c" then switch back to user mode, the later function is empty, please go fill it.


	// [3] Calculate the number of free frames in the physical memory. 
	//	Hint: Use function sys_calculate_free_frames() to calculate the free frames.
	// 	This function will switch to the kernel mode, calls the kernel function calculate_free_frames() in 
	//	"memory_manager.c" then switch back to user mode, the later function is empty, please go fill it.


	// [4] Check if the required number of frames available.
	// If available:
	// 	For each page in the range [ ptr_user_free_mem, ptr_user_free_mem + size ] do:
	// 		Make sure that the page is not mapped to an allocated frame 
	// 		Note: the first page of the range may be partially used by previous allocation (i.e. already allocated)
	// 		If the page is not mapped:
	// 			Allocate a frame from the physical memory, 
	// 			Map the page to the allocated frame
	// Hint: Use sys_get_page (void *virtual	_address, int perm) to perform these steps. This function will switch to kernel mode, calls the kernel function get_page (uint32* ptr_page_directory, void *virtual_address, int perm) in "memory_manager.c" then switch back to user mode, the later function is empty, please go fill it.


	// 	Update the ptr_user_free_mem.
	// 	Return pointer containing the virtual address of allocated space, the pointer should not exceed USER_HEAP_MAX - 1
	// Else:
	// 	Print error message and return NULL.


	return NULL	;
}

//=================================================================================//
//============================== BONUS FUNCTION ===================================//
//=================================================================================//

// freeHeap:
//	This function frees all the dynamic allocated space starting at USER_HEAP_START 
//	to ptr_user_free_mem
//	Steps:
//		1) Unmap all mapped pages in the range [USER_HEAP_START, ptr_user_free_mem]
//		2) Free all mapped page tables in this range
//		3) Set ptr_user_free_mem to USER_HEAP_START 
//	To do these steps, we need to switch to the kernel, unmap the pages and page tables
//	then switch back to the user again. 
//	Hint: Use function sys_freeMem(void* start_virtual_address, uint32 size) which 
//	will switch to the kernel mode, then calls 
//	freeMem(uint32* ptr_page_directory, void* start_virtual_address, uint32 size) in 
//	"memory_manager.c" then switch back to user mode, the later function is empty, 
//	please go fill it.

void freeHeap()
{
	//PROJECT 2008: your code here
	//	

	panic("freeHeap is not implemented yet");
}
