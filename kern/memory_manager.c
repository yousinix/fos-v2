/* See COPYRIGHT for copyright information. */
/*
KEY WORDS
==========
MACROS: 	STATIC_KERNEL_PHYSICAL_ADDRESS, STATIC_KERNEL_VIRTUAL_ADDRESS, PDX, PTX, CONSTRUCT_ENTRY, EXTRACT_ADDRESS, ROUNDUP, ROUNDDOWN, LIST_INIT, LIST_INSERT_HEAD, LIST_FIRST, LIST_REMOVE
CONSTANTS:	PAGE_SIZE, PERM_PRESENT, PERM_WRITEABLE, PERM_USER, KERNEL_STACK_TOP, KERNEL_STACK_SIZE, KERNEL_BASE, READ_ONLY_FRAMES_INFO, PHYS_IO_MEM, PHYS_EXTENDED_MEM, E_NO_MEM
VARIABLES:	ptr_free_mem, ptr_page_directory, phys_page_directory, phys_stack_bottom, Frame_Info, frames_info, free_frame_list, references, prev_next_info, size_of_extended_mem, number_of_frames, ptr_frame_info ,create, perm, va
FUNCTIONS:	to_physical_address, get_frame_info, tlb_invalidate
=====================================================================================================================================================================================================
 */

#include <kern/memory_manager.h>
#include <kern/file_manager.h>
#include <inc/x86.h>
#include <inc/mmu.h>
#include <inc/error.h>
#include <inc/string.h>
#include <inc/assert.h>

#include <kern/trap.h>

#include <kern/kclock.h>
#include <kern/user_environment.h>
#include <kern/sched.h>
#include <kern/kheap.h>
#include <kern/file_manager.h>

extern uint32 number_of_frames;	// Amount of physical memory (in frames_info)
extern uint32 size_of_base_mem;		// Amount of base memory (in bytes)
extern uint32 size_of_extended_mem;		// Amount of extended memory (in bytes)

inline uint32 env_table_ws_get_size(struct Env *e);
inline void env_table_ws_invalidate(struct Env* e, uint32 virtual_address);
inline void env_table_ws_set_entry(struct Env* e, uint32 entry_index, uint32 virtual_address);
inline void env_table_ws_clear_entry(struct Env* e, uint32 entry_index);
inline uint32 env_table_ws_get_virtual_address(struct Env* e, uint32 entry_index);
inline uint32 env_table_ws_is_entry_empty(struct Env* e, uint32 entry_index);
void env_table_ws_print(struct Env *curenv);

inline uint32 pd_is_table_used(struct Env *e, uint32 virtual_address);
inline void pd_set_table_unused(struct Env *e, uint32 virtual_address);
inline void pd_clear_page_dir_entry(struct Env *e, uint32 virtual_address);


// These variables are set in initialize_kernel_VM()
uint32* ptr_page_directory;		// Virtual address of boot time page directory
uint8* ptr_zero_page;		// Virtual address of zero page used by program loader to initialize extra segment zero memory (bss section) it to zero
uint8* ptr_temp_page;		// Virtual address of a page used by program loader to initialize segment last page fraction
uint32 phys_page_directory;		// Physical address of boot time page directory
char* ptr_free_mem;	// Pointer to next byte of free mem

struct Frame_Info* frames_info;		// Virtual address of physical frames_info array
struct Frame_Info* disk_frames_info;		// Virtual address of physical frames_info array
struct Linked_List free_frame_list;	// Free list of physical frames_info
struct Linked_List modified_frame_list;


///**************************** MAPPING KERNEL SPACE *******************************

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
	phys_page_directory = STATIC_KERNEL_PHYSICAL_ADDRESS(ptr_page_directory);

	//////////////////////////////////////////////////////////////////////
	// Map the kernel stack with VA range :
	//  [KERNEL_STACK_TOP-KERNEL_STACK_SIZE, KERNEL_STACK_TOP),
	// to physical address : "phys_stack_bottom".
	//     Permissions: kernel RW, user NONE
	// Your code goes here:
	boot_map_range(ptr_page_directory, KERNEL_STACK_TOP - KERNEL_STACK_SIZE, KERNEL_STACK_SIZE, STATIC_KERNEL_PHYSICAL_ADDRESS(ptr_stack_bottom), PERM_WRITEABLE) ;

	//////////////////////////////////////////////////////////////////////
	// Map all of physical memory at KERNEL_BASE.
	// i.e.  the VA range [KERNEL_BASE, 2^32) should map to
	//      the PA range [0, 2^32 - KERNEL_BASE)
	// We might not have 2^32 - KERNEL_BASE bytes of physical memory, but
	// we just set up the mapping anyway.
	// Permissions: kernel RW, user NONE
	// Your code goes here:

	//2016:
	//boot tables
	unsigned long long sva = KERNEL_BASE;
	unsigned int nTables=0;
	for (;sva < 0xFFFFFFFF;  sva += PTSIZE)
	{
		++nTables;
		boot_get_page_table(ptr_page_directory, (uint32)sva, 1);
	}
	//cprintf("nTables = %d\n", nTables);

	//////////////////////////////////////////////////////////////////////
	// Make 'frames_info' point to an array of size 'number_of_frames' of 'struct Frame_Info'.
	// The kernel uses this structure to keep track of physical frames;
	// 'number_of_frames' equals the number of physical frames in memory.  User-level
	// programs get read-only access to the array as well.
	// You must allocate the array yourself.
	// ************************************************************************************
	// /*2016: READ_ONLY_FRAMES_INFO not valid any more since it can't fit in 4 MB space*/
	//	Map this array read-only by the user at virtual address READ_ONLY_FRAMES_INFO
	// (ie. perm = PERM_USER | PERM_PRESENT)
	// ************************************************************************************
	// Permissions:
	//    - frames_info -- kernel RW, user NONE
	//    - the image mapped at READ_ONLY_FRAMES_INFO  -- kernel R, user R
	// Your code goes here:
	//cprintf("size of WorkingSetPage = %d\n",sizeof(struct WorkingSetPage));
	uint32 array_size;
	array_size = number_of_frames * sizeof(struct Frame_Info) ;
	frames_info = boot_allocate_space(array_size, PAGE_SIZE);
	memset(frames_info, 0, array_size);

	//2016: Not valid any more since the RAM size exceed the 64 MB limit. This lead to the
	// 		size of "frames_info" can exceed the 4 MB space for "READ_ONLY_FRAMES_INFO"
	//boot_map_range(ptr_page_directory, READ_ONLY_FRAMES_INFO, array_size, STATIC_KERNEL_PHYSICAL_ADDRESS(frames_info),PERM_USER) ;


	uint32 disk_array_size = PAGES_PER_FILE * sizeof(struct Frame_Info);
	disk_frames_info = boot_allocate_space(disk_array_size , PAGE_SIZE);
	memset(disk_frames_info , 0, disk_array_size);

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
	cprintf("Max Envs = %d\n",NENV);
	int envs_size = NENV * sizeof(struct Env) ;

	//allocate space for "envs" array aligned on 4KB boundary
	envs = boot_allocate_space(envs_size, PAGE_SIZE);
	memset(envs , 0, envs_size);

	//make the user to access this array by mapping it to UPAGES linear address (UPAGES is in User/Kernel space)
	boot_map_range(ptr_page_directory, UENVS, envs_size, STATIC_KERNEL_PHYSICAL_ADDRESS(envs), PERM_USER) ;

	//update permissions of the corresponding entry in page directory to make it USER with PERMISSION read only
	ptr_page_directory[PDX(UENVS)] = ptr_page_directory[PDX(UENVS)]|(PERM_USER|(PERM_PRESENT & (~PERM_WRITEABLE)));


#if USE_KHEAP
	{
		// MAKE SURE THAT THIS MAPPING HAPPENS AFTER ALL BOOT ALLOCATIONS (boot_allocate_space)
		// calls are fininshed, and no remaining data to be allocated for the kernel
		// map all used pages so far for the kernel
		boot_map_range(ptr_page_directory, KERNEL_BASE, (uint32)ptr_free_mem - KERNEL_BASE, 0, PERM_WRITEABLE) ;
	}
#else
	{
		boot_map_range(ptr_page_directory, KERNEL_BASE, 0xFFFFFFFF - KERNEL_BASE, 0, PERM_WRITEABLE) ;
	}
#endif
	// Check that the initial page directory has been set up correctly.
	check_boot_pgdir();

	memory_scarce_threshold_percentage = DEFAULT_MEM_SCARCE_PERCENTAGE;	// Memory remains plentiful till % of free frames gets below 25% of the memory space

	/*
	NOW: Turn off the segmentation by setting the segments' base to 0, and
	turn on the paging by setting the corresponding flags in control register 0 (cr0)
	 */
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
	ptr_free_mem = ROUNDUP(ptr_free_mem, align) ;

	//	Step 2: save current value of ptr_free_mem as allocated space
	void *ptr_allocated_mem;
	ptr_allocated_mem = ptr_free_mem ;

	//	Step 3: increase ptr_free_mem to record allocation
	ptr_free_mem += size ;

	//// 2016: Step 3.5: initialize allocated space by ZEROOOOOOOOOOOOOO
	//memset(ptr_allocated_mem, 0, size);

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
	//physical_address = ROUNDUP(physical_address, PAGE_SIZE) ;
	///we assume here that all addresses are given divisible by 4 KB, look at boot_allocate_space ...

	for (i = 0 ; i < size ; i += PAGE_SIZE)
	{
		uint32 *ptr_page_table = boot_get_page_table(ptr_page_directory, virtual_address, 1) ;
		uint32 index_page_table = PTX(virtual_address);
		//LOG_VARS("\nCONSTRUCT_ENTRY = %x",physical_address);
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

	//cprintf("boot d ind = %d, entry = %x\n",index_page_directory, page_directory_entry);
	uint32 phys_page_table = EXTRACT_ADDRESS(page_directory_entry);
	uint32 *ptr_page_table = STATIC_KERNEL_VIRTUAL_ADDRESS(phys_page_table);
	if (phys_page_table == 0)
	{
		if (create)
		{
			ptr_page_table = boot_allocate_space(PAGE_SIZE, PAGE_SIZE) ;
			phys_page_table = STATIC_KERNEL_PHYSICAL_ADDRESS(ptr_page_table);
			ptr_page_directory[index_page_directory] = CONSTRUCT_ENTRY(phys_page_table, PERM_PRESENT | PERM_WRITEABLE);
			return ptr_page_table ;
		}
		else
			return 0 ;
	}
	return ptr_page_table ;
}

///******************************* END of MAPPING KERNEL SPACE *******************************




///******************************* MAPPING USER SPACE *******************************

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

extern void initialize_disk_page_file();
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
	LIST_INIT(&modified_frame_list);

	frames_info[0].references = 1;
	frames_info[1].references = 1;
	frames_info[2].references = 1;
	ptr_zero_page = (uint8*) KERNEL_BASE+PAGE_SIZE;
	ptr_temp_page = (uint8*) KERNEL_BASE+2*PAGE_SIZE;
	i =0;
	for(;i<1024; i++)
	{
		ptr_zero_page[i]=0;
		ptr_temp_page[i]=0;
	}

	int range_end = ROUNDUP(PHYS_IO_MEM,PAGE_SIZE);

	for (i = 3; i < range_end/PAGE_SIZE; i++)
	{

		initialize_frame_info(&(frames_info[i]));
		//frames_info[i].references = 0;

		LIST_INSERT_HEAD(&free_frame_list, &frames_info[i]);
	}

	for (i = PHYS_IO_MEM/PAGE_SIZE ; i < PHYS_EXTENDED_MEM/PAGE_SIZE; i++)
	{
		frames_info[i].references = 1;
	}

	range_end = ROUNDUP(STATIC_KERNEL_PHYSICAL_ADDRESS(ptr_free_mem), PAGE_SIZE);

	for (i = PHYS_EXTENDED_MEM/PAGE_SIZE ; i < range_end/PAGE_SIZE; i++)
	{
		frames_info[i].references = 1;
	}

	for (i = range_end/PAGE_SIZE ; i < number_of_frames; i++)
	{
		initialize_frame_info(&(frames_info[i]));

		//frames_info[i].references = 0;
		LIST_INSERT_HEAD(&free_frame_list, &frames_info[i]);
	}

	initialize_disk_page_file();
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
//   If failed, it panic.
//
// Hint: use LIST_FIRST, LIST_REMOVE, and initialize_frame_info
// Hint: references should not be incremented

extern void env_free(struct Env *e);

int allocate_frame(struct Frame_Info **ptr_frame_info)
{
	*ptr_frame_info = LIST_FIRST(&free_frame_list);
	int c = 0;
	if (*ptr_frame_info == NULL)
	{
		//TODO: [PROJECT 2020 - BONUS5] Free RAM when it's FULL
		panic("ERROR: Kernel run out of memory... allocate_frame cannot find a free frame.\n");
		// When allocating new frame, if there's no free frame, then you should:
		//	1-	If any process has exited (those with status ENV_EXIT), then remove one or more of these exited processes from the main memory
		//	2-	otherwise, free at least 1 frame from the second list of the working set of EACH process

	}

	LIST_REMOVE(&free_frame_list,*ptr_frame_info);

	/******************* PAGE BUFFERING CODE *******************
	 ***********************************************************/

	if((*ptr_frame_info)->isBuffered)
	{
		pt_clear_page_table_entry((*ptr_frame_info)->environment,(*ptr_frame_info)->va);
		//pt_set_page_permissions((*ptr_frame_info)->environment->env_pgdir, (*ptr_frame_info)->va, 0, PERM_BUFFERED);
	}

	/**********************************************************
	 ***********************************************************/

	initialize_frame_info(*ptr_frame_info);
	return 0;
}

//
// Return a frame to the free_frame_list.
// (This function should only be called when ptr_frame_info->references reaches 0.)
//
void free_frame(struct Frame_Info *ptr_frame_info)
{
	/*2012: clear it to ensure that its members (env, isBuffered, ...) become NULL*/
	initialize_frame_info(ptr_frame_info);
	/*=============================================================================*/

	// Fill this function in
	LIST_INSERT_HEAD(&free_frame_list, ptr_frame_info);
	//LOG_STATMENT(cprintf("FN # %d FREED",to_frame_number(ptr_frame_info)));
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
// Stores address of page table entry in *ptr_page_table .
// Stores 0 if there is no such entry or on error.
//
// IT RETURNS:
//  TABLE_IN_MEMORY : if page table exists in main memory
//	TABLE_NOT_EXIST : if page table doesn't exist,
//

int get_page_table(uint32 *ptr_page_directory, const void *virtual_address, uint32 **ptr_page_table)
{
	//	cprintf("gpt .05\n");
	uint32 page_directory_entry = ptr_page_directory[PDX(virtual_address)];

	//	cprintf("gpt .07, page_directory_entry= %x \n",page_directory_entry);
	if(USE_KHEAP && !CHECK_IF_KERNEL_ADDRESS(virtual_address))
	{
		*ptr_page_table = (void *)kheap_virtual_address(EXTRACT_ADDRESS(page_directory_entry)) ;
		//cprintf("===>get_page_table: page_dir_entry = %x ptr_page_table = %x\n", page_directory_entry,*ptr_page_table);
	}
	else
	{
		*ptr_page_table = STATIC_KERNEL_VIRTUAL_ADDRESS(EXTRACT_ADDRESS(page_directory_entry)) ;
	}

	if ( (page_directory_entry & PERM_PRESENT) == PERM_PRESENT)
	{
		return TABLE_IN_MEMORY;
	}
	else if (page_directory_entry != 0) //the table exists but not in main mem, so it must be in sec mem
	{
		// Put the faulted address in CR2 and then
		// Call the fault_handler() to load the table in memory for us ...
		//		cprintf("gpt .1\n, %x page_directory_entry\n", page_directory_entry);
		lcr2((uint32)virtual_address) ;

		//		cprintf("gpt .12\n");
		fault_handler(NULL);

		//		cprintf("gpt .15\n");
		// now the page_fault_handler() should have returned successfully and updated the
		// directory with the new table frame number in memory
		page_directory_entry = ptr_page_directory[PDX(virtual_address)];
		if(USE_KHEAP && !CHECK_IF_KERNEL_ADDRESS(virtual_address))
		{
			*ptr_page_table = (void *)kheap_virtual_address(EXTRACT_ADDRESS(page_directory_entry)) ;
		}
		else
		{
			*ptr_page_table = STATIC_KERNEL_VIRTUAL_ADDRESS(EXTRACT_ADDRESS(page_directory_entry)) ;
		}

		return TABLE_IN_MEMORY;
	}
	else // there is no table for this va anywhere. This is a new table required, so check if the user want creation
	{
		//		cprintf("gpt .2\n");
		*ptr_page_table = 0;
		return TABLE_NOT_EXIST;
	}
}

void * create_page_table(uint32 *ptr_page_directory, const uint32 virtual_address)
{
	panic("this function is not required...!!");
	return NULL;
}

void __static_cpt(uint32 *ptr_page_directory, const uint32 virtual_address, uint32 **ptr_page_table)
{
	struct Frame_Info* ptr_new_frame_info;
	int err = allocate_frame(&ptr_new_frame_info) ;

	uint32 phys_page_table = to_physical_address(ptr_new_frame_info);
	*ptr_page_table = STATIC_KERNEL_VIRTUAL_ADDRESS(phys_page_table) ;
	ptr_new_frame_info->references = 1;
	ptr_page_directory[PDX(virtual_address)] = CONSTRUCT_ENTRY(phys_page_table, PERM_PRESENT | PERM_USER | PERM_WRITEABLE);
	//initialize new page table by 0's
	memset(*ptr_page_table , 0, PAGE_SIZE);
	tlbflush();
}
//
// Map the physical frame 'ptr_frame_info' at 'virtual_address'.
// The permissions (the low 12 bits) of the page table
//  entry should be set to 'perm|PERM_PRESENT'.
//
// Details
//   - If there is already a frame mapped at 'virtual_address', it should be unmaped
// using unmap_frame().
//   - If necessary, on demand, allocates a page table and inserts it into 'ptr_page_directory'.
//   - ptr_frame_info->references should be incremented if the insertion succeeds
//
// RETURNS:
//   0 on success
//
// Hint: implement using get_page_table() and unmap_frame().
//
int map_frame(uint32 *ptr_page_directory, struct Frame_Info *ptr_frame_info, void *virtual_address, int perm)
{
	// Fill this function in
	uint32 physical_address = to_physical_address(ptr_frame_info);
	uint32 *ptr_page_table;
	if( get_page_table(ptr_page_directory, virtual_address, &ptr_page_table) == TABLE_NOT_EXIST)
	{
		/*==========================================================================================
		// OLD WRONG SOLUTION
		//=====================
		//// initiate a read instruction for an address inside the wanted table.
		//// this will generate a page fault, that will cause page_fault_handler() to
		//// create the table in memory for us ...
		//char dummy_char = *((char*)virtual_address) ;
		//// a page fault is created now and page_fault_handler() should start handling the fault ...

		//// now the page_fault_handler() should have returned successfully and updated the
		//// directory with the new table frame number in memory
		//uint32 page_directory_entry;
		//page_directory_entry = ptr_page_directory[PDX(virtual_address)];
		//ptr_page_table = STATIC_KERNEL_VIRTUAL_ADDRESS(EXTRACT_ADDRESS(page_directory_entry)) ;
		=============================================================================================*/
#if USE_KHEAP
		{
			ptr_page_table = create_page_table(ptr_page_directory, (uint32)virtual_address);
			//cprintf("======>page table created using kheap at dir = %x PT = %x\n", ptr_page_directory[PDX(virtual_address)], ptr_page_table);
			uint32* ptr_page_table2 =NULL;
			//cprintf("======> After the table created at %x\n\n", get_page_table(ptr_page_directory, virtual_address,&ptr_page_table2));
		}
#else
		{
			__static_cpt(ptr_page_directory, (uint32)virtual_address, &ptr_page_table);
		}
#endif

	}

	//cprintf("NOW .. map add = %x ptr_page_table = %x PTX(virtual_address) = %d\n", virtual_address, ptr_page_table,PTX(virtual_address));
	uint32 page_table_entry = ptr_page_table[PTX(virtual_address)];

	/*OLD WRONG SOLUTION
	if( EXTRACT_ADDRESS(page_table_entry) != physical_address)
	{
		if( page_table_entry != 0)
		{
			unmap_frame(ptr_page_directory , virtual_address);
		}
		ptr_frame_info->references++;
		ptr_page_table[PTX(virtual_address)] = CONSTRUCT_ENTRY(physical_address , perm | PERM_PRESENT);

	}*/

	/*NEW'15 CORRECT SOLUTION*/
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
	//cprintf(".gfi .1\n %x, %x, %x, \n", ptr_page_directory, virtual_address, ptr_page_table);
	uint32 ret =  get_page_table(ptr_page_directory, virtual_address, ptr_page_table) ;
	//cprintf(".gfi .15\n");
	if((*ptr_page_table) != 0)
	{
		uint32 index_page_table = PTX(virtual_address);
		//cprintf(".gfi .2\n");
		uint32 page_table_entry = (*ptr_page_table)[index_page_table];
		if( page_table_entry != 0)
		{
			//cprintf(".gfi .3\n");
			return to_frame_info( EXTRACT_ADDRESS ( page_table_entry ) );
		}
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
		if (ptr_frame_info->isBuffered && !CHECK_IF_KERNEL_ADDRESS((uint32)virtual_address))
			cprintf("Freeing BUFFERED frame at va %x!!!\n", virtual_address) ;
		decrement_references(ptr_frame_info);
		ptr_page_table[PTX(virtual_address)] = 0;
		tlb_invalidate(ptr_page_directory, virtual_address);
	}
}


/*/this function should be called only in the env_create() for creating the page table if not exist
 * (without causing page fault as the normal map_frame())*/
// Map the physical frame 'ptr_frame_info' at 'virtual_address'.
// The permissions (the low 12 bits) of the page table
//  entry should be set to 'perm|PERM_PRESENT'.
//
// Details
//   - If there is already a frame mapped at 'virtual_address', it should be unmaped
// using unmap_frame().
//   - If necessary, on demand, allocates a page table and inserts it into 'ptr_page_directory'.
//   - ptr_frame_info->references should be incremented if the insertion succeeds
//
// RETURNS:
//   0 on success
//
//
int loadtime_map_frame(uint32 *ptr_page_directory, struct Frame_Info *ptr_frame_info, void *virtual_address, int perm)
{
	uint32 physical_address = to_physical_address(ptr_frame_info);
	uint32 *ptr_page_table;

	uint32 page_directory_entry = ptr_page_directory[PDX(virtual_address)];

	if(USE_KHEAP && !CHECK_IF_KERNEL_ADDRESS(virtual_address))
	{
		ptr_page_table = (uint32*)kheap_virtual_address(EXTRACT_ADDRESS(page_directory_entry)) ;
	}
	else
	{
		ptr_page_table = STATIC_KERNEL_VIRTUAL_ADDRESS(EXTRACT_ADDRESS(page_directory_entry)) ;
	}

	//if page table not exist, create it in memory and link it with the directory
	if (page_directory_entry == 0)
	{
#if USE_KHEAP
		{
			ptr_page_table = create_page_table(ptr_page_directory, (uint32)virtual_address);
		}
#else
		{
			__static_cpt(ptr_page_directory, (uint32)virtual_address, &ptr_page_table);
		}
#endif
	}

	ptr_frame_info->references++;
	ptr_page_table[PTX(virtual_address)] = CONSTRUCT_ENTRY(physical_address , perm | PERM_PRESENT);

	return 0;
}


///****************************************************************************************///
///******************************* END OF MAPPING USER SPACE ******************************///
///****************************************************************************************///


//======================================================
/// functions used for malloc() and freeHeap()
//======================================================

// [1] allocateMem

void allocateMem(struct Env* e, uint32 virtual_address, uint32 size)
{
	//TODO: [PROJECT 2020 - [2] User Heap] allocateMem() [Kernel Side]
	// Write your code here, remove the panic and write your code
	panic("allocateMem() is not implemented yet...!!");

	//This function should allocate ALL pages of the required range in the PAGE FILE
	//and allocate NOTHING in the main memory
}


// [2] freeMem

void freeMem(struct Env* e, uint32 virtual_address, uint32 size)
{

	//TODO: [PROJECT 2020 - [2] User Heap] freeMem() [Kernel Side]
	// Write your code here, remove the panic and write your code
	panic("freeMem() is not implemented yet...!!");

	//This function should:
	//1. Free ALL pages of the given range from the Page File
	//2. Free ONLY pages that are resident in the working set from the memory
	//3. Removes ONLY the empty page tables (i.e. not used) (no pages are mapped in the table)
}

void __freeMem_with_buffering(struct Env* e, uint32 virtual_address, uint32 size)
{
	panic("this function is not required...!!");
}

//================= [BONUS] =====================
// [3] moveMem

void moveMem(struct Env* e, uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
{
	//TODO: [PROJECT 2020 - BONUS2] User Heap Realloc [Kernel Side]
	//your code is here, remove the panic and write your code
	panic("moveMem() is not implemented yet...!!");

	// This function should move all pages from "src_virtual_address" to "dst_virtual_address"
	// with the given size
	// After finished, the src_virtual_address must no longer be accessed/exist in either page file
	// or main memory
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

// calculate_required_frames:
// calculates the new allocatino size required for given address+size,
// we are not interested in knowing if pages or tables actually exist in memory or the page file,
// we are interested in knowing whether they are allocated or not.
uint32 calculate_required_frames(uint32* ptr_page_directory, uint32 start_virtual_address, uint32 size)
{
	LOG_STATMENT(cprintf("calculate_required_frames: Starting at address %x",start_virtual_address));
	//calculate the required page tables
	uint32 number_of_tables = 0;

	long i = 0;
	uint32 current_virtual_address = ROUNDDOWN(start_virtual_address, PAGE_SIZE*1024);

	for(; current_virtual_address < (start_virtual_address+size); current_virtual_address+= PAGE_SIZE*1024)
	{
		uint32 *ptr_page_table;
		get_page_table(ptr_page_directory, (void*) current_virtual_address, &ptr_page_table);

		if(ptr_page_table == 0)
		{
			(number_of_tables)++;
		}
	}

	//calc the required page frames
	uint32 number_of_pages = 0;
	current_virtual_address = ROUNDDOWN(start_virtual_address, PAGE_SIZE);

	for(; current_virtual_address < (start_virtual_address+size); current_virtual_address+= PAGE_SIZE)
	{
		uint32 *ptr_page_table;
		if (get_frame_info(ptr_page_directory, (void*) current_virtual_address, &ptr_page_table) == 0)
		{
			(number_of_pages)++;
		}
	}

	//return total number of frames
	LOG_STATMENT(cprintf("calculate_required_frames: Done!"));
	return number_of_tables+number_of_pages;
}



// calculate_available_frames:
struct freeFramesCounters calculate_available_frames()
{
	//DETECTING LOOP inside the list
	//================================
	/*	struct  Frame_Info * slowPtr = LIST_FIRST(&free_frame_list);
	struct  Frame_Info * fastPtr = LIST_FIRST(&free_frame_list);


	while (slowPtr && fastPtr) {
		fastPtr = LIST_NEXT(fastPtr); // advance the fast pointer
		if (fastPtr == slowPtr) // and check if its equal to the slow pointer
		{
			cprintf("loop detected in freelist\n");
			break;
		}

		if (fastPtr == NULL) {
			break; // since fastPtr is NULL we reached the tail
		}

		fastPtr = LIST_NEXT(fastPtr); //advance and check again
		if (fastPtr == slowPtr) {
			cprintf("loop detected in freelist\n");
			break;
		}

		slowPtr = LIST_NEXT(slowPtr); // advance the slow pointer only once
	}
	cprintf("finished loop detction\n");
	 */
	//calculate the free frames from the free frame list
	struct Frame_Info *ptr;
	uint32 totalFreeUnBuffered = 0 ;
	uint32 totalFreeBuffered = 0 ;
	uint32 totalModified = 0 ;


	LIST_FOREACH(ptr, &free_frame_list)
	{
		if (ptr->isBuffered)
			totalFreeBuffered++ ;
		else
			totalFreeUnBuffered++ ;
	}



	LIST_FOREACH(ptr, &modified_frame_list)
	{
		totalModified++ ;
	}


	struct freeFramesCounters counters ;
	counters.freeBuffered = totalFreeBuffered ;
	counters.freeNotBuffered = totalFreeUnBuffered ;
	counters.modified = totalModified;
	return counters;
}

//2018
// calculate_free_frames:
uint32 calculate_free_frames()
{
	return LIST_SIZE(&free_frame_list);
}



///============================================================================================
/// Dealing with environment working set

inline uint32 env_page_ws_get_size(struct Env *e)
{
	int i=0, counter=0;
	for(;i<e->page_WS_max_size; i++) if(e->ptr_pageWorkingSet[i].empty == 0) counter++;
	return counter;
}

inline void env_page_ws_invalidate(struct Env* e, uint32 virtual_address)
{
	int i=0;
	for(;i<e->page_WS_max_size; i++)
	{
		if(ROUNDDOWN(e->ptr_pageWorkingSet[i].virtual_address,PAGE_SIZE) == ROUNDDOWN(virtual_address,PAGE_SIZE))
		{
			env_page_ws_clear_entry(e, i);
			break;
		}
	}
}

inline void env_page_ws_set_entry(struct Env* e, uint32 entry_index, uint32 virtual_address)
{
	assert(entry_index >= 0 && entry_index < e->page_WS_max_size);
	assert(virtual_address >= 0 && virtual_address < USER_TOP);
	e->ptr_pageWorkingSet[entry_index].virtual_address = ROUNDDOWN(virtual_address,PAGE_SIZE);
	e->ptr_pageWorkingSet[entry_index].empty = 0;

	return;
}

inline void env_page_ws_clear_entry(struct Env* e, uint32 entry_index)
{
	assert(entry_index >= 0 && entry_index < (e->page_WS_max_size));
	e->ptr_pageWorkingSet[entry_index].virtual_address = 0;
	e->ptr_pageWorkingSet[entry_index].empty = 1;
}

inline uint32 env_page_ws_get_virtual_address(struct Env* e, uint32 entry_index)
{
	assert(entry_index >= 0 && entry_index < (e->page_WS_max_size));
	return ROUNDDOWN(e->ptr_pageWorkingSet[entry_index].virtual_address,PAGE_SIZE);
}

inline uint32 env_page_ws_is_entry_empty(struct Env* e, uint32 entry_index)
{
	return e->ptr_pageWorkingSet[entry_index].empty;
}

void env_page_ws_print(struct Env *curenv)
{
	uint32 i;
	cprintf("PAGE WS:\n");
	for(i=0; i< (curenv->page_WS_max_size); i++ )
	{
		if (curenv->ptr_pageWorkingSet[i].empty)
		{
			cprintf("EMPTY LOCATION");
			if(i==curenv->page_last_WS_index )
			{
				cprintf("		<--");
			}
			cprintf("\n");
			continue;
		}
		uint32 virtual_address = curenv->ptr_pageWorkingSet[i].virtual_address;

		uint32 perm = pt_get_page_permissions(curenv, virtual_address) ;
		char isModified = ((perm&PERM_MODIFIED) ? 1 : 0);
		char isUsed= ((perm&PERM_USED) ? 1 : 0);
		char isBuffered= ((perm&PERM_BUFFERED) ? 1 : 0);


		cprintf("address @ %d = %x",i, curenv->ptr_pageWorkingSet[i].virtual_address);

		cprintf(", used= %d, modified= %d, buffered= %d", isUsed, isModified, isBuffered) ;

		if(i==curenv->page_last_WS_index )
		{
			cprintf(" <--");
		}
		cprintf("\n");
	}
}

// Table Working Set =========================================================

void env_table_ws_print(struct Env *curenv)
{
	uint32 i;
	cprintf("---------------------------------------------------\n");
	cprintf("TABLE WS:\n");
	for(i=0; i< __TWS_MAX_SIZE; i++ )
	{
		if (curenv->__ptr_tws[i].empty)
		{
			cprintf("EMPTY LOCATION");
			if(i==curenv->table_last_WS_index )
			{
				cprintf("		<--");
			}
			cprintf("\n");
			continue;
		}
		uint32 virtual_address = curenv->__ptr_tws[i].virtual_address;
		cprintf("env address at %d = %x",i, curenv->__ptr_tws[i].virtual_address);

		cprintf(", used bit = %d", pd_is_table_used(curenv, virtual_address));
		if(i==curenv->table_last_WS_index )
		{
			cprintf(" <--");
		}
		cprintf("\n");
	}
}

inline uint32 env_table_ws_get_size(struct Env *e)
{
	int i=0, counter=0;
	for(;i<__TWS_MAX_SIZE; i++) if(e->__ptr_tws[i].empty == 0) counter++;
	return counter;
}

inline void env_table_ws_invalidate(struct Env* e, uint32 virtual_address)
{
	int i=0;
	for(;i<__TWS_MAX_SIZE; i++)
	{
		if(ROUNDDOWN(e->__ptr_tws[i].virtual_address,PAGE_SIZE*1024) == ROUNDDOWN(virtual_address,PAGE_SIZE*1024))
		{
			env_table_ws_clear_entry(e, i);
			break;
		}
	}
}

inline void env_table_ws_set_entry(struct Env* e, uint32 entry_index, uint32 virtual_address)
{
	assert(entry_index >= 0 && entry_index < __TWS_MAX_SIZE);
	assert(virtual_address >= 0 && virtual_address < USER_TOP);
	e->__ptr_tws[entry_index].virtual_address = ROUNDDOWN(virtual_address,PAGE_SIZE*1024);
	e->__ptr_tws[entry_index].empty = 0;

	//e->__ptr_tws[entry_index].time_stamp = time;
	return;
}

inline void env_table_ws_clear_entry(struct Env* e, uint32 entry_index)
{
	assert(entry_index >= 0 && entry_index < __TWS_MAX_SIZE);
	e->__ptr_tws[entry_index].virtual_address = 0;
	e->__ptr_tws[entry_index].empty = 1;
}

inline uint32 env_table_ws_get_virtual_address(struct Env* e, uint32 entry_index)
{
	assert(entry_index >= 0 && entry_index < __TWS_MAX_SIZE);
	return ROUNDDOWN(e->__ptr_tws[entry_index].virtual_address,PAGE_SIZE*1024);
}


inline uint32 env_table_ws_is_entry_empty(struct Env* e, uint32 entry_index)
{
	return e->__ptr_tws[entry_index].empty;
}

void addTableToTableWorkingSet(struct Env *e, uint32 tableAddress)
{
	tableAddress = ROUNDDOWN(tableAddress, PAGE_SIZE*1024);
	e->__ptr_tws[e->table_last_WS_index].virtual_address = tableAddress;
	e->__ptr_tws[e->table_last_WS_index].empty = 0;

	e->table_last_WS_index ++;
	e->table_last_WS_index %= __TWS_MAX_SIZE;
}
///=================================================================================================




///****************************************************************************************///
///******************************* PAGE BUFFERING FUNCTIONS ******************************///
///****************************************************************************************///

void bufferList_add_page(struct Linked_List* bufferList,struct Frame_Info *ptr_frame_info)
{

	//cprintf("inserting frame_info %x, mod_first = %x, mod_last = %x\n", ptr_frame_info , LIST_FIRST(&modified_frame_list), LIST_LAST(&modified_frame_list));
	/*	if (bufferList == &modified_frame_list)

	{
		struct Frame_Info *ptr;

		if (ptr_frame_info == (struct Frame_Info*) 0xf02ebd78)
						{
							cprintf("the frame is buffered into MODIFIED list *******************************\n");
						}

		LIST_FOREACH(ptr, &free_frame_list)
		{
			if (ptr == ptr_frame_info)
			{
				cprintf("kashaftak!! modif\n\n");
				cprintf("common frame = %x\n", ptr_frame_info);
				cprintf("frame page_va = %x\n", ptr->va);
				cprintf("page permissions = %x\n", pt_get_page_permissions(ptr->environment->env_pgdir,ptr->va));
				break;
			}
		}
	}
	else
	{
		struct Frame_Info *ptr;

		if (ptr_frame_info == (struct Frame_Info*) 0xf02ebd78)
								{
									cprintf("the frame is buffered into FREE list *******************************\n");
								}

		LIST_FOREACH(ptr, &modified_frame_list)
		{
			if (ptr == ptr_frame_info)
			{
				cprintf("kashaftak!! free\n\n");
				cprintf("common frame = %x\n", ptr_frame_info);
				cprintf("frame page_va = %x\n", ptr->va);
				cprintf("page permissions = %x\n", pt_get_page_permissions(ptr->environment->env_pgdir,ptr->va));
				break;
			}
		}

	}
	 */
	LIST_INSERT_TAIL(bufferList, ptr_frame_info);
}
void bufferlist_remove_page(struct Linked_List* bufferList, struct Frame_Info *ptr_frame_info)
{
	LIST_REMOVE(bufferList, ptr_frame_info);
}



///============================================================================================
/// Dealing with page and page table entry flags

inline uint32 pd_is_table_used(struct Env* ptr_env, uint32 virtual_address)
{
	return ( (ptr_env->env_page_directory[PDX(virtual_address)] & PERM_USED) == PERM_USED ? 1 : 0);
}

inline void pd_set_table_unused(struct Env* ptr_env, uint32 virtual_address)
{
	ptr_env->env_page_directory[PDX(virtual_address)] &= (~PERM_USED);
	tlb_invalidate((void *)NULL, (void *)virtual_address);
}

inline void pd_clear_page_dir_entry(struct Env* ptr_env, uint32 virtual_address)
{
	uint32 * ptr_pgdir = ptr_env->env_page_directory ;
	ptr_pgdir[PDX(virtual_address)] = 0 ;
	tlbflush();
}

extern int __pf_write_env_table( struct Env* ptr_env, uint32 virtual_address, uint32* tableKVirtualAddress);
extern int __pf_read_env_table(struct Env* ptr_env, uint32 virtual_address, uint32* tableKVirtualAddress);

inline void pt_set_page_permissions(struct Env* ptr_env, uint32 virtual_address, uint32 permissions_to_set, uint32 permissions_to_clear)
{
	uint32 * ptr_pgdir = ptr_env->env_page_directory ;
	uint32* ptr_page_table;
	//if(get_page_table(ptr_pgdir, (void *)virtual_address, &ptr_page_table) == TABLE_NOT_EXIST)
	//	panic("function pt_set_page_unmodified() called with invalid virtual address\n") ;

	uint32 	page_directory_entry = ptr_pgdir[PDX(virtual_address)] ;
	if ( (page_directory_entry & PERM_PRESENT) == PERM_PRESENT)
	{
		if(USE_KHEAP && !CHECK_IF_KERNEL_ADDRESS(virtual_address))
		{
			ptr_page_table = (uint32*)kheap_virtual_address(EXTRACT_ADDRESS(page_directory_entry)) ;
		}
		else
		{
			ptr_page_table = STATIC_KERNEL_VIRTUAL_ADDRESS(EXTRACT_ADDRESS(page_directory_entry)) ;
		}
		ptr_page_table[PTX(virtual_address)] |= (permissions_to_set);
		ptr_page_table[PTX(virtual_address)] &= (~permissions_to_clear);

	}
	else if (page_directory_entry != 0) //the table exists but not in main mem, so it must be in sec mem
	{
		//cprintf("Warning %d: pt_is_page_modified() is called while the page table is on disk!!\n", ++cnt);
		//Temporary read the table from page file into main memory
		int success = __pf_read_env_table(ptr_env, virtual_address, (void*) ptr_temp_page);
		ptr_page_table = (uint32*) ptr_temp_page;
		if(success == E_TABLE_NOT_EXIST_IN_PF)
			panic("pt_set_page_permissions: table not found in PF when expected to find one !. please revise your table fault\
			handling code");

		ptr_page_table[PTX(virtual_address)] |= (permissions_to_set);
		ptr_page_table[PTX(virtual_address)] &= (~permissions_to_clear);

		__pf_write_env_table(ptr_env, virtual_address, (void*) ptr_temp_page);
	}
	else
	{
		//cprintf("[%s] va = %x\n", ptr_env->prog_name, virtual_address) ;
		panic("function pt_set_page_permissions() called with invalid virtual address. The corresponding page table doesn't exist\n") ;
	}

	tlb_invalidate((void *)NULL, (void *)virtual_address);
}

inline void pt_clear_page_table_entry(struct Env* ptr_env, uint32 virtual_address)
{
	uint32 * ptr_pgdir = ptr_env->env_page_directory ;
	uint32* ptr_page_table;
	//if(get_page_table(ptr_pgdir, (void *)virtual_address, &ptr_page_table) == TABLE_NOT_EXIST)
	//	panic("function pt_set_page_unmodified() called with invalid virtual address\n") ;

	uint32 	page_directory_entry = ptr_pgdir[PDX(virtual_address)] ;
	if ((page_directory_entry & PERM_PRESENT) == PERM_PRESENT)
	{
		if(USE_KHEAP && !CHECK_IF_KERNEL_ADDRESS(virtual_address))
		{
			ptr_page_table = (uint32*)kheap_virtual_address(EXTRACT_ADDRESS(page_directory_entry)) ;
		}
		else
		{
			ptr_page_table = STATIC_KERNEL_VIRTUAL_ADDRESS(EXTRACT_ADDRESS(page_directory_entry)) ;
		}

		ptr_page_table[PTX(virtual_address)] = 0 ;
	}
	else if (page_directory_entry != 0) //the table exists but not in main mem, so it must be in sec mem
	{
		//cprintf("Warning %d: pt_is_page_modified() is called while the page table is on disk!!\n", ++cnt);
		//Temporary read the table from page file into main memory

		int success = __pf_read_env_table(ptr_env, virtual_address, (void*) ptr_temp_page);
		ptr_page_table = (uint32*) ptr_temp_page;
		if(success == E_TABLE_NOT_EXIST_IN_PF)
			panic("pt_clear_page_table_entry: table not found in PF when expected to find one !. please revise your table fault\
			handling code");

		ptr_page_table[PTX(virtual_address)] = 0 ;

		__pf_write_env_table(ptr_env, virtual_address, (void*) ptr_temp_page);
	}
	else
		panic("function pt_clear_page_table_entry() called with invalid virtual address. The corresponding page table doesn't exist\n") ;


	tlb_invalidate((void *)NULL, (void *)virtual_address);
}

inline uint32 pt_get_page_permissions(struct Env* ptr_env, uint32 virtual_address )
{
	uint32 * ptr_pgdir = ptr_env->env_page_directory ;
	uint32* ptr_page_table;

	uint32 	page_directory_entry = ptr_pgdir[PDX(virtual_address)] ;
	if ( (page_directory_entry & PERM_PRESENT) == PERM_PRESENT)
	{
		if(USE_KHEAP && !CHECK_IF_KERNEL_ADDRESS(virtual_address))
		{
			ptr_page_table = (uint32*)kheap_virtual_address(EXTRACT_ADDRESS(page_directory_entry)) ;
		}
		else
		{
			ptr_page_table = STATIC_KERNEL_VIRTUAL_ADDRESS(EXTRACT_ADDRESS(page_directory_entry)) ;
		}
	}
	else if (page_directory_entry != 0) //the table exists but not in main mem, so it must be in sec mem
	{
		//cprintf("Warning %d: pt_is_page_modified() is called while the page table is on disk!!\n", ++cnt);
		//Temporary read the table from page file into main memory
		int success = __pf_read_env_table(ptr_env, virtual_address, (void*) ptr_temp_page);
		ptr_page_table = (uint32*) ptr_temp_page;
		if(success == E_TABLE_NOT_EXIST_IN_PF)
			panic("pt_get_page_permissions: table not found in PF when expected to find one !. please revise your table fault\
			handling code");
	}
	else
		return 0;
	//panic("function pt_get_page_permissions() called with invalid virtual address. The corresponding page table doesn't exist\n") ;

	//	if(get_page_table(ptr_pgdir, (void *)virtual_address, &ptr_page_table) == TABLE_NOT_EXIST)
	//		panic("function pt_is_page_modified() called with invalid virtual address\n") ;

	return (ptr_page_table[PTX(virtual_address)] & 0x00000FFF);
}


//=============================================================
// 2014 - edited in 2017
//=============================================================
// [1] if KHEAP = 1: Create the frames_storage by allocating a PAGE for its directory
inline uint32* create_frames_storage()
{
	uint32* frames_storage = (void *)kmalloc(PAGE_SIZE);
	if(frames_storage == NULL)
	{
		panic("NOT ENOUGH KERNEL HEAP SPACE");
	}
	return frames_storage;
}
// [2] Add a frame info to the storage of frames at the given index
inline void add_frame_to_storage(uint32* frames_storage, struct Frame_Info* ptr_frame_info, uint32 index)
{
	uint32 va = index * PAGE_SIZE ;
	uint32 *ptr_page_table;
	int r = get_page_table(frames_storage, (void*) va, &ptr_page_table);
	if(r == TABLE_NOT_EXIST)
	{
#if USE_KHEAP
		{
			ptr_page_table = create_page_table(frames_storage, (uint32)va);
		}
#else
		{
			__static_cpt(frames_storage, (uint32)va, &ptr_page_table);

		}
#endif
	}
	ptr_page_table[PTX(va)] = CONSTRUCT_ENTRY(to_physical_address(ptr_frame_info), 0 | PERM_PRESENT);
}

// [3] Get a frame info from the storage of frames at the given index
inline struct Frame_Info* get_frame_from_storage(uint32* frames_storage, uint32 index)
{
	struct Frame_Info* ptr_frame_info;
	uint32 *ptr_page_table ;
	uint32 va = index * PAGE_SIZE ;
	ptr_frame_info = get_frame_info(frames_storage, (void*) va, &ptr_page_table);
	return ptr_frame_info;
}

// [4] Clear the storage of frames
inline void clear_frames_storage(uint32* frames_storage)
{
	int fourMega = 1024 * PAGE_SIZE ;
	int i ;
	for (i = 0 ; i < 1024 ; i++)
	{
		if (frames_storage[i] != 0)
		{
#if USE_KHEAP
			{
				kfree((void*)kheap_virtual_address(EXTRACT_ADDRESS(frames_storage[i])));
			}
#else
			{
				free_frame(to_frame_info(EXTRACT_ADDRESS(frames_storage[i])));
			}
#endif
			frames_storage[i] = 0;
		}
	}
}
//********************************************************************************//
/*2015*/
void setUHeapPlacementStrategyFIRSTFIT(){_UHeapPlacementStrategy = UHP_PLACE_FIRSTFIT;}
void setUHeapPlacementStrategyBESTFIT(){_UHeapPlacementStrategy = UHP_PLACE_BESTFIT;}
void setUHeapPlacementStrategyNEXTFIT(){_UHeapPlacementStrategy = UHP_PLACE_NEXTFIT;}
void setUHeapPlacementStrategyWORSTFIT(){_UHeapPlacementStrategy = UHP_PLACE_WORSTFIT;}

uint32 isUHeapPlacementStrategyFIRSTFIT(){if(_UHeapPlacementStrategy == UHP_PLACE_FIRSTFIT) return 1; return 0;}
uint32 isUHeapPlacementStrategyBESTFIT(){if(_UHeapPlacementStrategy == UHP_PLACE_BESTFIT) return 1; return 0;}
uint32 isUHeapPlacementStrategyNEXTFIT(){if(_UHeapPlacementStrategy == UHP_PLACE_NEXTFIT) return 1; return 0;}
uint32 isUHeapPlacementStrategyWORSTFIT(){if(_UHeapPlacementStrategy == UHP_PLACE_WORSTFIT) return 1; return 0;}

//********************************************************************************//
/*2017*/
void setKHeapPlacementStrategyCONTALLOC(){_KHeapPlacementStrategy = KHP_PLACE_CONTALLOC;}
void setKHeapPlacementStrategyFIRSTFIT(){_KHeapPlacementStrategy = KHP_PLACE_FIRSTFIT;}
void setKHeapPlacementStrategyBESTFIT(){_KHeapPlacementStrategy = KHP_PLACE_BESTFIT;}
void setKHeapPlacementStrategyNEXTFIT(){_KHeapPlacementStrategy = KHP_PLACE_NEXTFIT;}
void setKHeapPlacementStrategyWORSTFIT(){_KHeapPlacementStrategy = KHP_PLACE_WORSTFIT;}

uint32 isKHeapPlacementStrategyCONTALLOC(){if(_KHeapPlacementStrategy == KHP_PLACE_CONTALLOC) return 1; return 0;}
uint32 isKHeapPlacementStrategyFIRSTFIT(){if(_KHeapPlacementStrategy == KHP_PLACE_FIRSTFIT) return 1; return 0;}
uint32 isKHeapPlacementStrategyBESTFIT(){if(_KHeapPlacementStrategy == KHP_PLACE_BESTFIT) return 1; return 0;}
uint32 isKHeapPlacementStrategyNEXTFIT(){if(_KHeapPlacementStrategy == KHP_PLACE_NEXTFIT) return 1; return 0;}
uint32 isKHeapPlacementStrategyWORSTFIT(){if(_KHeapPlacementStrategy == KHP_PLACE_WORSTFIT) return 1; return 0;}



