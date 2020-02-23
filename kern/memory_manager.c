/* See COPYRIGHT for copyright information.

KEY WORDS
==========
MACROS: 	K_PHYSICAL_ADDRESS, K_VIRTUAL_ADDRESS, PDX, PTX, CONSTRUCT_ENTRY, EXTRACT_ADDRESS, ROUNDUP, ROUNDDOWN, LIST_INIT, LIST_INSERT_HEAD, LIST_FIRST, LIST_REMOVE
CONSTANTS:	PAGE_SIZE, PERM_PRESENT, PERM_WRITEABLE, PERM_USER, KERNEL_STACK_TOP, KERNEL_STACK_SIZE, KERNEL_BASE, READ_ONLY_FRAMES_INFO, PHYS_IO_MEM, PHYS_EXTENDED_MEM, E_NO_MEM
VARIABLES:	ptr_free_mem, ptr_page_directory, phys_page_directory, phys_stack_bottom, Frame_Info, frames_info, free_frame_list, references, prev_next_info, size_of_extended_mem, number_of_frames, ptr_frame_info ,create, perm, va
FUNCTIONS:	to_physical_address, get_frame_info, tlb_invalidate
=====================================================================================================================================================================================================
 */

#include <kern/memory_manager.h>
#include <inc/x86.h>
#include <inc/mmu.h>
#include <inc/error.h>
#include <inc/string.h>
#include <inc/assert.h>

#include <kern/kclock.h>
#include <kern/user_environment.h>

extern uint32 number_of_frames;	// Amount of physical memory (in frames_info)
extern uint32 size_of_base_mem;		// Amount of base memory (in bytes)
extern uint32 size_of_extended_mem;		// Amount of extended memory (in bytes)

// These variables are set in initialize_kernel_VM()
uint32* ptr_page_directory;		// Virtual address of boot time page directory
uint32 phys_page_directory;		// Physical address of boot time page directory
char* ptr_free_mem;	// Pointer to next byte of free mem

struct Frame_Info* frames_info;		// Virtual address of physical frames_info array
struct Linked_List free_frame_list;	// Free list of physical frames_info


//==================== MAPPING KERNEL SPACE ==============================

// Set up a two-level page table:
//    ptr_page_directory is the virtual address of the page directory
//    phys_page_directory is the physical adresss of the page directory
// Then turn on paging.  Then effectively turn off segmentation.
// (i.e., the segment base addrs are set to zero).
// 
// This function only sets up the kernel part of the address space
// (ie. addresses >= USER_TOP).  The user part of the address space
// will be setup later.
//
// From USER_TOP to USER_LIMIT, the user is allowed to read but not write.
// Above USER_LIMIT the user cannot read (or write).

void initialize_kernel_VM()
{
	// Remove this line when you're ready to test this function.
	//panic("initialize_kernel_VM: This function is not finished\n");

	//////////////////////////////////////////////////////////////////////
	// create initial page directory.

	ptr_page_directory = boot_allocate_space(PAGE_SIZE, PAGE_SIZE);
	memset(ptr_page_directory, 0, PAGE_SIZE);
	phys_page_directory = K_PHYSICAL_ADDRESS(ptr_page_directory);

	//////////////////////////////////////////////////////////////////////
	// Map the kernel stack with VA range :
	//  [KERNEL_STACK_TOP-KERNEL_STACK_SIZE, KERNEL_STACK_TOP), 
	// to physical address : "phys_stack_bottom".
	//     Permissions: kernel RW, user NONE
	// Your code goes here:
	boot_map_range(ptr_page_directory, KERNEL_STACK_TOP - KERNEL_STACK_SIZE, KERNEL_STACK_SIZE, K_PHYSICAL_ADDRESS(ptr_stack_bottom), PERM_WRITEABLE) ;

	//////////////////////////////////////////////////////////////////////
	// Map all of physical memory at KERNEL_BASE.
	// i.e.  the VA range [KERNEL_BASE, 2^32) should map to
	//      the PA range [0, 2^32 - KERNEL_BASE)
	// We might not have 2^32 - KERNEL_BASE bytes of physical memory, but
	// we just set up the mapping anyway.
	// Permissions: kernel RW, user NONE
	// Your code goes here: 
	boot_map_range(ptr_page_directory, KERNEL_BASE, 0xFFFFFFFF - KERNEL_BASE, 0, PERM_WRITEABLE) ;

	//////////////////////////////////////////////////////////////////////
	// Make 'frames_info' point to an array of size 'number_of_frames' of 'struct Frame_Info'.
	// The kernel uses this structure to keep track of physical frames;
	// 'number_of_frames' equals the number of physical frames in memory.  User-level
	// programs get read-only access to the array as well.
	// You must allocate the array yourself.
	// Map this array read-only by the user at virtual address READ_ONLY_FRAMES_INFO
	// (ie. perm = PERM_USER | PERM_PRESENT)
	// Permissions:
	//    - frames_info -- kernel RW, user NONE
	//    - the image mapped at READ_ONLY_FRAMES_INFO  -- kernel R, user R
	// Your code goes here:
	uint32 array_size;
	array_size = number_of_frames * sizeof(struct Frame_Info) ;
	frames_info = boot_allocate_space(array_size, PAGE_SIZE);
	boot_map_range(ptr_page_directory, READ_ONLY_FRAMES_INFO, array_size, K_PHYSICAL_ADDRESS(frames_info), PERM_USER) ;


	// This allows the kernel & user to access any page table entry using a
	// specified VA for each: VPT for kernel and UVPT for User.
	setup_listing_to_all_page_tables_entries();

	//////////////////////////////////////////////////////////////////////
	// Make 'envs' point to an array of size 'NENV' of 'struct Env'.
	// Map this array read-only by the user at linear address UENVS
	// (ie. perm = PTE_U | PTE_P).
	// Permissions:
	//    - envs itself -- kernel RW, user NONE
	//    - the image of envs mapped at UENVS  -- kernel R, user R

	// LAB 3: Your code here.
	int envs_size = NENV * sizeof(struct Env) ;

	//allocate space for "envs" array aligned on 4KB boundary
	envs = boot_allocate_space(envs_size, PAGE_SIZE);

	//make the user to access this array by mapping it to UPAGES linear address (UPAGES is in User/Kernel space)
	boot_map_range(ptr_page_directory, UENVS, envs_size, K_PHYSICAL_ADDRESS(envs), PERM_USER) ;

	//update permissions of the corresponding entry in page directory to make it USER with PERMISSION read only
	ptr_page_directory[PDX(UENVS)] = ptr_page_directory[PDX(UENVS)]|(PERM_USER|(PERM_PRESENT & (~PERM_WRITEABLE)));


	// Check that the initial page directory has been set up correctly.
	check_boot_pgdir();

	// NOW: Turn off the segmentation by setting the segments' base to 0, and
	// turn on the paging by setting the corresponding flags in control register 0 (cr0)
	turn_on_paging() ;
}

//
// Allocate "size" bytes of physical memory aligned on an
// "align"-byte boundary.  Align must be a power of two.
// Return the start kernel virtual address of the allocated space.
// Returned memory is uninitialized.
//
// If we're out of memory, boot_allocate_space should panic.
// It's too early to run out of memory.
// This function may ONLY be used during boot time,
// before the free_frame_list has been set up.
// 
void* boot_allocate_space(uint32 size, uint32 align)
		{
	extern char end_of_kernel[];	

	// Initialize ptr_free_mem if this is the first time.
	// 'end_of_kernel' is a symbol automatically generated by the linker,
	// which points to the end of the kernel-
	// i.e., the first virtual address that the linker
	// did not assign to any kernel code or global variables.
	if (ptr_free_mem == 0)
		ptr_free_mem = end_of_kernel;

	// Your code here:
	//	Step 1: round ptr_free_mem up to be aligned properly
	ptr_free_mem = ROUNDUP(ptr_free_mem, PAGE_SIZE) ;

	//	Step 2: save current value of ptr_free_mem as allocated space
	void *ptr_allocated_mem;
	ptr_allocated_mem = ptr_free_mem ;

	//	Step 3: increase ptr_free_mem to record allocation
	ptr_free_mem += size ;

	//	Step 4: return allocated space
	return ptr_allocated_mem ;

		}

//
// Map [virtual_address, virtual_address+size) of virtual address space to
// physical [physical_address, physical_address+size)
// in the page table rooted at ptr_page_directory.
// "size" is a multiple of PAGE_SIZE.
// Use permission bits perm|PERM_PRESENT for the entries.
//
// This function may ONLY be used during boot time,
// before the free_frame_list has been set up.
//
void boot_map_range(uint32 *ptr_page_directory, uint32 virtual_address, uint32 size, uint32 physical_address, int perm)
{
	int i = 0 ;
	physical_address = ROUNDUP(physical_address, PAGE_SIZE) ;
	for (i = 0 ; i < size ; i += PAGE_SIZE)
	{
		uint32 *ptr_page_table = boot_get_page_table(ptr_page_directory, virtual_address, 1) ;
		uint32 index_page_table = PTX(virtual_address);
		ptr_page_table[index_page_table] = CONSTRUCT_ENTRY(physical_address, perm | PERM_PRESENT) ;
		physical_address += PAGE_SIZE ;
		virtual_address += PAGE_SIZE ;
	}
}

//
// Given ptr_page_directory, a pointer to a page directory,
// traverse the 2-level page table structure to find
// the page table for "virtual_address".
// Return a pointer to the table.
//
// If the relevant page table doesn't exist in the page directory:
//	- If create == 0, return 0.
//	- Otherwise allocate a new page table, install it into ptr_page_directory,
//	  and return a pointer into it.
//        (Questions: What data should the new page table contain?
//	  And what permissions should the new ptr_page_directory entry have?)
//
// This function allocates new page tables as needed.
// 
// boot_get_page_table cannot fail.  It's too early to fail.
// This function may ONLY be used during boot time,
// before the free_frame_list has been set up.
//
uint32* boot_get_page_table(uint32 *ptr_page_directory, uint32 virtual_address, int create)
		{
	uint32 index_page_directory = PDX(virtual_address);
	uint32 page_directory_entry = ptr_page_directory[index_page_directory];

	uint32 phys_page_table = EXTRACT_ADDRESS(page_directory_entry);
	uint32 *ptr_page_table = K_VIRTUAL_ADDRESS(phys_page_table);
	if (phys_page_table == 0)
	{
		if (create)
		{
			ptr_page_table = boot_allocate_space(PAGE_SIZE, PAGE_SIZE) ;
			phys_page_table = K_PHYSICAL_ADDRESS(ptr_page_table);
			ptr_page_directory[index_page_directory] = CONSTRUCT_ENTRY(phys_page_table, PERM_PRESENT | PERM_WRITEABLE);
			return ptr_page_table ;
		}
		else
			return 0 ;
	}
	return ptr_page_table ;
		}

//==================== END of MAPPING KERNEL SPACE ==============================




//========================== MAPPING USER SPACE ==============================

// --------------------------------------------------------------
// Tracking of physical frames.
// The 'frames_info' array has one 'struct Frame_Info' entry per physical frame.
// frames_info are reference counted, and free frames are kept on a linked list.
// --------------------------------------------------------------

// Initialize paging structure and free_frame_list.
// After this point, ONLY use the functions below
// to allocate and deallocate physical memory via the free_frame_list,
// and NEVER use boot_allocate_space() or the related boot-time functions above.
//
void initialize_paging()
{
	// The example code here marks all frames_info as free.
	// However this is not truly the case.  What memory is free?
	//  1) Mark frame 0 as in use.
	//     This way we preserve the real-mode IDT and BIOS structures
	//     in case we ever need them.  (Currently we don't, but...)
	//  2) Mark the rest of base memory as free.
	//  3) Then comes the IO hole [PHYS_IO_MEM, PHYS_EXTENDED_MEM).
	//     Mark it as in use so that it can never be allocated.      
	//  4) Then extended memory [PHYS_EXTENDED_MEM, ...).
	//     Some of it is in use, some is free. Where is the kernel?
	//     Which frames are used for page tables and other data structures?
	//
	// Change the code to reflect this.
	int i;
	LIST_INIT(&free_frame_list);

	frames_info[0].references = 1;

	int range_end = ROUNDUP(PHYS_IO_MEM,PAGE_SIZE);

	for (i = 1; i < range_end/PAGE_SIZE; i++)
	{
		frames_info[i].references = 0;
		LIST_INSERT_HEAD(&free_frame_list, &frames_info[i]);
	}

	for (i = PHYS_IO_MEM/PAGE_SIZE ; i < PHYS_EXTENDED_MEM/PAGE_SIZE; i++)
	{
		frames_info[i].references = 1;
	}

	range_end = ROUNDUP(K_PHYSICAL_ADDRESS(ptr_free_mem), PAGE_SIZE);

	for (i = PHYS_EXTENDED_MEM/PAGE_SIZE ; i < range_end/PAGE_SIZE; i++)
	{
		frames_info[i].references = 1;
	}

	for (i = range_end/PAGE_SIZE ; i < number_of_frames; i++)
	{
		frames_info[i].references = 0;
		LIST_INSERT_HEAD(&free_frame_list, &frames_info[i]);
	}
}

//
// Initialize a Frame_Info structure.
// The result has null links and 0 references.
// Note that the corresponding physical frame is NOT initialized!
//
void initialize_frame_info(struct Frame_Info *ptr_frame_info)
{
	memset(ptr_frame_info, 0, sizeof(*ptr_frame_info));
}

//
// Allocates a physical frame.
// Does NOT set the contents of the physical frame to zero -
// the caller must do that if necessary.
//
// *ptr_frame_info -- is set to point to the Frame_Info struct of the
// newly allocated frame
//
// RETURNS 
//   0 -- on success
//   E_NO_MEM -- otherwise
//
// Hint: use LIST_FIRST, LIST_REMOVE, and initialize_frame_info
// Hint: references should not be incremented
int allocate_frame(struct Frame_Info **ptr_frame_info)
{
	// Fill this function in	
	*ptr_frame_info = LIST_FIRST(&free_frame_list);
	if(*ptr_frame_info == NULL)
		return E_NO_MEM;

	LIST_REMOVE(*ptr_frame_info);
	initialize_frame_info(*ptr_frame_info);
	return 0;
}

//
// Return a frame to the free_frame_list.
// (This function should only be called when ptr_frame_info->references reaches 0.)
//
void free_frame(struct Frame_Info *ptr_frame_info)
{
	// Fill this function in
	LIST_INSERT_HEAD(&free_frame_list, ptr_frame_info);
}

//
// Decrement the reference count on a frame
// freeing it if there are no more references.
//
void decrement_references(struct Frame_Info* ptr_frame_info)
{
	if (--(ptr_frame_info->references) == 0)
		free_frame(ptr_frame_info);
}

//
// This is like "boot_get_page_table()" with a different allocate function:
// namely, it should use allocate_frame() instead of boot_allocate_space().
// Unlike boot_get_page_table(), get_page_table() can fail, so we have to
// return "ptr_page_table" via a pointer parameter.
//
// Stores address of page table entry in *ptr_page_table .
// Stores 0 if there is no such entry or on error.
// 
// RETURNS: 
//   0 on success
//   E_NO_MEM, if page table couldn't be allocated
//
// Hint: you can use "to_physical_address()" to turn a Frame_Info*
// into the physical address of the frame it refers to. 

int get_page_table(uint32 *ptr_page_directory, const void *virtual_address, int create, uint32 **ptr_page_table)
{
	// Fill this function in
	uint32 page_directory_entry = ptr_page_directory[PDX(virtual_address)];

	*ptr_page_table = K_VIRTUAL_ADDRESS(EXTRACT_ADDRESS(page_directory_entry)) ;

	if (page_directory_entry == 0)
	{
		if (create)
		{
			struct Frame_Info* ptr_frame_info;
			int err = allocate_frame(&ptr_frame_info) ;
			if(err == E_NO_MEM)
			{
				*ptr_page_table = 0;
				return E_NO_MEM;
			}

			uint32 phys_page_table = to_physical_address(ptr_frame_info);
			*ptr_page_table = K_VIRTUAL_ADDRESS(phys_page_table) ;

			//initialize new page table by 0's
			memset(*ptr_page_table , 0, PAGE_SIZE);

			ptr_frame_info->references = 1;
			ptr_page_directory[PDX(virtual_address)] = CONSTRUCT_ENTRY(phys_page_table, PERM_PRESENT | PERM_USER | PERM_WRITEABLE);
		}
		else
		{
			*ptr_page_table = 0;
			return 0;
		}
	}	
	return 0;
}

//
// Map the physical frame 'ptr_frame_info' at 'virtual_address'.
// The permissions (the low 12 bits) of the page table
//  entry should be set to 'perm|PERM_PRESENT'.
//
// Details
//   - If there is already a frame mapped at 'virtual_address', it should be unmaped
// using unmap_frame().
//   - If necesary, on demand, allocates a page table and inserts it into 'ptr_page_directory'.
//   - ptr_frame_info->references should be incremented if the insertion succeeds
//
// RETURNS: 
//   0 on success
//   E_NO_MEM, if page table couldn't be allocated
//
// Hint: implement using get_page_table() and unmap_frame().
//
int map_frame(uint32 *ptr_page_directory, struct Frame_Info *ptr_frame_info, void *virtual_address, int perm)
{
	// Fill this function in
	uint32 physical_address = to_physical_address(ptr_frame_info);
	uint32 *ptr_page_table;
	if( get_page_table(ptr_page_directory, virtual_address, 1, &ptr_page_table) == 0)
	{
		uint32 page_table_entry = ptr_page_table[PTX(virtual_address)];

		//If already mapped
		if ((page_table_entry & PERM_PRESENT) == PERM_PRESENT)
		{
			//on this pa, then do nothing
			if (EXTRACT_ADDRESS(page_table_entry) == physical_address)
				return 0;
			//on another pa, then unmap it
			else
				unmap_frame(ptr_page_directory , virtual_address);
		}
		ptr_frame_info->references++;
		ptr_page_table[PTX(virtual_address)] = CONSTRUCT_ENTRY(physical_address , perm | PERM_PRESENT);

		return 0;
	}	
	return E_NO_MEM;
}

//
// Return the frame mapped at 'virtual_address'.
// If the page table entry corresponding to 'virtual_address' exists, then we store a pointer to the table in 'ptr_page_table'
// This is used by 'unmap_frame()'
// but should not be used by other callers.
//
// Return 0 if there is no frame mapped at virtual_address.
//
// Hint: implement using get_page_table() and get_frame_info().
//
struct Frame_Info * get_frame_info(uint32 *ptr_page_directory, void *virtual_address, uint32 **ptr_page_table)
		{
	// Fill this function in	
	uint32 ret =  get_page_table(ptr_page_directory, virtual_address, 0, ptr_page_table) ;
	if((*ptr_page_table) != 0)
	{	
		uint32 index_page_table = PTX(virtual_address);
		uint32 page_table_entry = (*ptr_page_table)[index_page_table];
		if( page_table_entry != 0)	
			return to_frame_info( EXTRACT_ADDRESS ( page_table_entry ) );
		return 0;
	}
	return 0;
		}

//
// Unmaps the physical frame at 'virtual_address'.
//
// Details:
//   - The references count on the physical frame should decrement.
//   - The physical frame should be freed if the 'references' reaches 0.
//   - The page table entry corresponding to 'virtual_address' should be set to 0.
//     (if such a page table exists)
//   - The TLB must be invalidated if you remove an entry from
//	   the page directory/page table.
//
// Hint: implement using get_frame_info(),
// 	tlb_invalidate(), and decrement_references().
//
void unmap_frame(uint32 *ptr_page_directory, void *virtual_address)
{
	// Fill this function in
	uint32 *ptr_page_table;
	struct Frame_Info* ptr_frame_info = get_frame_info(ptr_page_directory, virtual_address, &ptr_page_table);
	if( ptr_frame_info != 0 )
	{
		decrement_references(ptr_frame_info);
		ptr_page_table[PTX(virtual_address)] = 0;
		tlb_invalidate(ptr_page_directory, virtual_address);
	}	
}

//========================== END OF MAPPING USER SPACE ==============================
//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================




//======================================================
// functions used as helpers for malloc() and freeHeap()
//======================================================
//[1] get_page: 
//	it should allocate one frame and map it to the given virtual address
//	if the virtual address is already mapped, then it return 0
// 	Return 0 on success, < 0 on error.  Errors are:
//		E_INVAL if virtual_address >= USER_TOP.
//		E_INVAL if perm is not containing PERM_USER.
//		E_NO_MEM if there's no memory to allocate the new page,
//		or to allocate any necessary page tables.
// 	HINT: 	remember to free the allocated frame if there is no space 
//		for the necessary page tables

int get_page(uint32* ptr_page_directory, void *virtual_address, int perm)
{
	// PROJECT 2008: Your code here.
	panic("get_page function is not completed yet") ;

	//[1] check virtual address to be < USER_TOP
	// return E_INVAL if not

	//[2] check the value of perm to contain PERM_USER 
	// return E_INVAL if not

	//[3] check if the page containing the "virtual_address" is already mapped or not
	// return 0 if the page is already mapped
	// else:
	//	Allocate a frame from the physical memory, 
	//	Map the page to the allocated frame
	//	if there is no free space , return E_NO_MEM
	//	else return 0				 

	return 0 ;
}

//[2] calculate_required_frames: 
uint32 calculate_required_frames(uint32* ptr_page_directory, uint32 start_virtual_address, uint32 size)
{
	// PROJECT 2008: Your code here.
	panic("calculate_required_frames function is not completed yet") ;

	//calculate the required page tables	


	//calc the required page frames

	//return total number of frames  
	return 0; 
}


//[3] calculate_free_frames:

uint32 calculate_free_frames()
{
	// PROJECT 2008: Your code here.
	//panic("calculate_free_frames function is not completed yet") ;

	//calculate the free frames from the free frame list
	struct Frame_Info *ptr;
	uint32 cnt = 0 ; 
	LIST_FOREACH(ptr, &free_frame_list)
	{
		cnt++ ;
	}
	return cnt;
}

//[4] freeMem: 
//	This function is used to frees all pages and page tables that are mapped on
//	range [ virtual_address, virtual_address + size ]
//	Steps:
//		1) Unmap all mapped pages in the range [virtual_address, virtual_address + size ]
//		2) Free all mapped page tables in this range

void freeMem(uint32* ptr_page_directory, void *virtual_address, uint32 size)
{
	// PROJECT 2008: Your code here.
	panic("freeMem function is not completed yet") ;
}
