/* See COPYRIGHT for copyright information. */

#include <inc/x86.h>
#include <inc/mmu.h>
#include <inc/error.h>
#include <inc/string.h>
#include <inc/assert.h>
#include <inc/elf.h>

#include <kern/user_environment.h>
#include <kern/memory_manager.h>
#include <kern/trap.h>
#include <kern/command_prompt.h>
#include <kern/helpers.h>

struct Env *envs = NULL;		// All environments
struct Env *curenv = NULL;	        // The current env
static struct Env_list env_free_list;	// Free Environment list

#define ENVGENSHIFT	12		// >= LOGNENV

//Contains information about each program segment (e.g. start address, size, virtual address...)
//It will be used below in "env_create" to load each program segment into the user environment
struct ProgramSegment {
	uint8 *ptr_start;
	uint32 size_in_file;
	uint32 size_in_memory;
	uint8 *virtual_address;

	// for use only with PROGRAM_SEGMENT_FOREACH	
	uint32 segment_id; 
};

// Used inside the PROGRAM_SEGMENT_FOREACH macro to get the first program segment 
// and then iterate on the next ones
struct ProgramSegment* PROGRAM_SEGMENT_NEXT(struct ProgramSegment* seg, uint8* ptr_program_start);
struct ProgramSegment PROGRAM_SEGMENT_FIRST( uint8* ptr_program_start);

// Used inside "env_create" function to get information about each program segment inside the user program 
#define	PROGRAM_SEGMENT_FOREACH(Seg, ptr_program_start)					\
		struct ProgramSegment* first; \
		struct ProgramSegment tmp; \
		tmp = (PROGRAM_SEGMENT_FIRST(ptr_program_start));	 \
		first = &tmp; \
		if(first->segment_id == -1) first = NULL;\
		Seg = first; \
		for (;				\
		Seg;							\
		Seg = PROGRAM_SEGMENT_NEXT(Seg,ptr_program_start) )

// Helper functions to be used below
void complete_environment_initialization(struct Env* e);
void set_environment_entry_point(struct UserProgramInfo *ptr_user_program_info);

//=========================================================
//PROJECT 2008: your code is here
// To add FOS support for new user program, just add the appropriate lines like below

//The input for any DECLARE_START_OF macro must be the ".c" filename of the user program 
DECLARE_START_OF(fos_helloWorld)
DECLARE_START_OF(fos_add)
DECLARE_START_OF(fos_alloc)
DECLARE_START_OF(fos_input)
DECLARE_START_OF(game)


//User Programs Table
//The input for any PTR_START_OF macro must be the ".c" filename of the user program
struct UserProgramInfo userPrograms[] = {
		{ "fos_helloWorld", "Created by FOS team, fos@nowhere.com", PTR_START_OF(fos_helloWorld), 0 },
		{ "fos_add", "Created by FOS team, fos@nowhere.com", PTR_START_OF(fos_add), 0},
		{ "fos_alloc", "Created by FOS team, fos@nowhere.com", PTR_START_OF(fos_alloc), 0},
		{ "fos_input", "Created by FOS team, fos@nowhere.com", PTR_START_OF(fos_input), 0},
		{ "fos_game", "Created by FOS team, fos@nowhere.com", PTR_START_OF(game), 0},
};

//=========================================================

// To be used as extrern in other files 
struct UserProgramInfo* ptr_UserPrograms = &userPrograms[0];

// Number of user programs in the program table
int NUM_USER_PROGS = (sizeof(userPrograms)/sizeof(userPrograms[0]));

//
// Allocates and initializes a new environment.
// On success, the new environment is stored in *e.
//
// Returns 0 on success, < 0 on failure.  Errors include:
//	E_NO_FREE_ENV if all NENVS environments are allocated
//
int allocate_environment(struct Env** e)
{	
	if (!(*e = LIST_FIRST(&env_free_list)))
		return E_NO_FREE_ENV;
	return 0;
}

// Free the given environment "e", simply by adding it to the free environment list.
void free_environment(struct Env* e)
{
	curenv = NULL;	
	// return the environment to the free list
	e->env_status = ENV_FREE;
	LIST_INSERT_HEAD(&env_free_list, e);
}



//
// Allocate length bytes of physical memory for environment e,
// and map it at virtual address va in the environment's address space.
// Does not zero or otherwise initialize the mapped pages in any way.
// Pages should be writable by user and kernel.
//
// if the allocation failed, return E_NO_MEM 
// otherwise return 0
//
static int program_segment_alloc_map(struct Env *e, void *va, uint32 length)
{
	//TODO: LAB6 Hands-on: fill this function. 
	//Comment the following line
	panic("Function is not implemented yet!");

	//You should round round "va + length" up, and "va" down.
	//your code here ...
	
	return 0;
}

//
// Allocates a new env and loads the named user program into it.
struct UserProgramInfo* env_create(char* user_program_name)
{
	//[1] get pointer to the start of the "user_program_name" program in memory
	// Hint: use "get_user_program_info" function, 
	// you should set the following "ptr_program_start" by the start address of the user program 
	uint8* ptr_program_start = 0; 

	struct UserProgramInfo* ptr_user_program_info =get_user_program_info(user_program_name);

	if (ptr_user_program_info == 0)
		return NULL ;

	ptr_program_start = ptr_user_program_info->ptr_start ;

	//[2] allocate new environment, (from the free environment list)
	//if there's no one, return NULL
	// Hint: use "allocate_environment" function
	struct Env* e = NULL;
	if(allocate_environment(&e) == E_NO_FREE_ENV)
	{
		return 0;
	}

	//=========================================================
	//TODO: LAB6 Hands-on: fill this part. 
	//Comment the following line
	panic("env_create: directory creation is not implemented yet!");

	//[3] allocate a frame for the page directory, Don't forget to set the references of the allocated frame.
	//if there's no free space, return NULL
	//your code here . . .
	

	//[4] copy kernel directory into the new directory
	//your code here . . .
	

	//[5] set e->env_pgdir to page directory virtual address
	//and e->env_cr3 to page directory physical address.
	//your code here
	
	
	//============================================================

	//Complete other environment initializations, (envID, status and most of registers)
	complete_environment_initialization(e);

	//[6] update the UserProgramInfo in userPrograms[] corresponding to this program
	ptr_user_program_info->environment = e;

	// We want to load the program into the user virtual space
	// each program is constructed from one or more segments,
	// each segment has the following information grouped in "struct ProgramSegment"
	//	1- uint8 *ptr_start: 	start address of this segment in memory 
	//	2- uint32 size_in_file: size occupied by this segment inside the program file, 
	//	3- uint32 size_in_memory: actual size required by this segment in memory
	// 	usually size_in_file < or = size_in_memory 
	//	4- uint8 *virtual_address: start virtual address that this segment should be copied to it  

	//switch to user page directory
	// rcr3() reads cr3, lcr3() loads cr3
	int32 kern_phys_pgdir = rcr3() ;
	lcr3(e->env_cr3) ;

	//load each program segment into user virtual space
	struct ProgramSegment* seg = NULL;  //use inside PROGRAM_SEGMENT_FOREACH as current segment information	

	PROGRAM_SEGMENT_FOREACH(seg, ptr_program_start)
	{
		//============================================================
		//[7] allocate space for current program segment and map it at
		//seg->virtual_address
		// if program_segment_alloc_map() returns E_NO_MEM, call env_free() to     free all environment memory,
		// zero the UserProgramInfo* ptr->environment then return NULL

		//Hands On 2: implementation of function program_segment_alloc_map()
		int ret = program_segment_alloc_map(e, (void *)seg->virtual_address, seg->size_in_memory) ;

		if (ret == E_NO_MEM)
		{
			cprintf("Load program failed: no enough memory\n\nProgram will be unloaded now...\n\n");
			env_free(e);
			ptr_user_program_info->environment = NULL;
			return NULL;
		}
		//============================================================

		//[8] copy program segment from (seg->ptr_start) to
		//(seg->virtual_address) with size seg->size_in_file
		uint8 *src_ptr = (uint8 *)(seg->ptr_start) ;
		uint8 *dst_ptr = (uint8 *) seg->virtual_address;

		int i ;

		for(i = 0 ; i < seg->size_in_file; i++)
		{
			*dst_ptr = *src_ptr ;
			dst_ptr++ ;
			src_ptr++ ;
		}

		//Initialize the rest of the program segment the rest
		//(seg->size_in_memory - seg->size_in_file) bytes
		//By Zero
		for(i = seg->size_in_file ; i < seg->size_in_memory ; i++)
		{
			*dst_ptr = 0 ;
			dst_ptr++;
		}
	}

	//[9] now set the entry point of the environment
	set_environment_entry_point(ptr_user_program_info);


	//[10] Allocate and map one page for the program's initial stack
	// at virtual address USTACKTOP - PAGE_SIZE.
	// if there is no free memory, call env_free() to free all env. memory,
	// zero the UserProgramInfo* ptr->environment then return NULL

	struct Frame_Info *pp = NULL;
	if (allocate_frame(&pp) == E_NO_MEM)
	{
		cprintf("Load program failed: no enough memory\n\nProgram will be unloaded now...\n\n");
		env_free(e);
		ptr_user_program_info->environment = NULL;
		return NULL;
	}

	// map the allocated page
	void* ptr_user_stack_bottom = (void *)(USTACKTOP - PAGE_SIZE);
	int ret = map_frame(e->env_pgdir, pp, ptr_user_stack_bottom, PERM_USER|PERM_WRITEABLE);

	if (ret == E_NO_MEM)
	{
		cprintf("Load program failed: no enough memory\n\nProgram will be unloaded now...\n\n");
		env_free(e);
		ptr_user_program_info->environment = NULL;
		return NULL;
	}

	//initialize new page by 0's
	memset(ptr_user_stack_bottom, 0, PAGE_SIZE);

	//[11] switch back to the page directory exists before segment loading
	lcr3(kern_phys_pgdir) ;

	return ptr_user_program_info;
}

// Used to run the given environment "e", simply by 
// context switch from curenv to env e.
//  (This function does not return.)
//
void env_run(struct Env *e)
{
	if(curenv != e)
	{		
		curenv = e ;
		curenv->env_runs++ ;
		lcr3(curenv->env_cr3) ;	
	}	
	env_pop_tf(&(curenv->env_tf));
}

//
// Frees environment "e" and all memory it uses.
// 
void env_free(struct Env *e)
{
	panic("env_free function is not completed yet") ;

	// [1] Unmap all mapped pages in the user portion of the environment (i.e. below USER_TOP)

	// [2] Free all mapped page tables in the user portion of the environment

	// [3] free the page directory of the environment

	// [4] switch back to the kernel page directory

	// [5] free the environment (return it back to the free environment list)
	// Hint: use free_environment()
}


//====================================================================================
//====================================================================================
//====================================================================================
//====================================================================================
//====================================================================================


// Mark all environments in 'envs' as free, set their env_ids to 0,
// and insert them into the env_free_list.
// Insert in reverse order, so that the first call to allocate_environment()
// returns envs[0].
//
void
env_init(void)
{	
	int iEnv = NENV-1;
	for(; iEnv >= 0; iEnv--)
	{
		envs[iEnv].env_status = ENV_FREE;
		envs[iEnv].env_id = 0;
		LIST_INSERT_HEAD(&env_free_list, &envs[iEnv]);	
	}
}

void complete_environment_initialization(struct Env* e)
{	
	//VPT and UVPT map the env's own page table, with
	//different permissions.
	e->env_pgdir[PDX(VPT)]  = e->env_cr3 | PERM_PRESENT | PERM_WRITEABLE;
	e->env_pgdir[PDX(UVPT)] = e->env_cr3 | PERM_PRESENT | PERM_USER;

	int32 generation;	
	// Generate an env_id for this environment.
	generation = (e->env_id + (1 << ENVGENSHIFT)) & ~(NENV - 1);
	if (generation <= 0)	// Don't create a negative env_id.
		generation = 1 << ENVGENSHIFT;
	e->env_id = generation | (e - envs);

	// Set the basic status variables.
	e->env_parent_id = 0;//parent_id;
	e->env_status = ENV_RUNNABLE;
	e->env_runs = 0;

	// Clear out all the saved register state,
	// to prevent the register values
	// of a prior environment inhabiting this Env structure
	// from "leaking" into our new environment.
	memset(&e->env_tf, 0, sizeof(e->env_tf));

	// Set up appropriate initial values for the segment registers.
	// GD_UD is the user data segment selector in the GDT, and 
	// GD_UT is the user text segment selector (see inc/memlayout.h).
	// The low 2 bits of each segment register contains the
	// Requestor Privilege Level (RPL); 3 means user mode.

	e->env_tf.tf_ds = GD_UD | 3;
	e->env_tf.tf_es = GD_UD | 3;
	e->env_tf.tf_ss = GD_UD | 3;
	e->env_tf.tf_esp = (uint32*)USTACKTOP;
	e->env_tf.tf_cs = GD_UT | 3;
	// You will set e->env_tf.tf_eip later.

	// commit the allocation
	LIST_REMOVE(e);	
	return ;
}

struct ProgramSegment* PROGRAM_SEGMENT_NEXT(struct ProgramSegment* seg, uint8* ptr_program_start)
				{
	int index = (*seg).segment_id++;

	struct Proghdr *ph, *eph; 
	struct Elf * pELFHDR = (struct Elf *)ptr_program_start ; 
	if (pELFHDR->e_magic != ELF_MAGIC) 
		panic("Matafa2nash 3ala Keda"); 
	ph = (struct Proghdr *) ( ((uint8 *) ptr_program_start) + pELFHDR->e_phoff);

	while (ph[(*seg).segment_id].p_type != ELF_PROG_LOAD && ((*seg).segment_id < pELFHDR->e_phnum)) (*seg).segment_id++;	
	index = (*seg).segment_id;

	if(index < pELFHDR->e_phnum)
	{
		(*seg).ptr_start = (uint8 *) ptr_program_start + ph[index].p_offset;
		(*seg).size_in_memory =  ph[index].p_memsz;
		(*seg).size_in_file = ph[index].p_filesz;
		(*seg).virtual_address = (uint8*)ph[index].p_va;
		return seg;
	}
	return 0;
				}

struct ProgramSegment PROGRAM_SEGMENT_FIRST( uint8* ptr_program_start)
{
	struct ProgramSegment seg;
	seg.segment_id = 0;

	struct Proghdr *ph, *eph; 
	struct Elf * pELFHDR = (struct Elf *)ptr_program_start ; 
	if (pELFHDR->e_magic != ELF_MAGIC) 
		panic("Matafa2nash 3ala Keda"); 
	ph = (struct Proghdr *) ( ((uint8 *) ptr_program_start) + pELFHDR->e_phoff);
	while (ph[(seg).segment_id].p_type != ELF_PROG_LOAD && ((seg).segment_id < pELFHDR->e_phnum)) (seg).segment_id++;
	int index = (seg).segment_id;

	if(index < pELFHDR->e_phnum)
	{	
		(seg).ptr_start = (uint8 *) ptr_program_start + ph[index].p_offset;
		(seg).size_in_memory =  ph[index].p_memsz;
		(seg).size_in_file = ph[index].p_filesz;
		(seg).virtual_address = (uint8*)ph[index].p_va;
		return seg;
	}
	seg.segment_id = -1;
	return seg;
}

struct UserProgramInfo* get_user_program_info(char* user_program_name)
				{
	int i;
	for (i = 0; i < NUM_USER_PROGS; i++) {
		if (strcmp(user_program_name, userPrograms[i].name) == 0)
			break;
	}
	if(i==NUM_USER_PROGS) 
	{
		cprintf("Unknown user program '%s'\n", user_program_name);
		return 0;
	}

	return &userPrograms[i];
				}

struct UserProgramInfo* get_user_program_info_by_env(struct Env* e)
				{
	int i;
	for (i = 0; i < NUM_USER_PROGS; i++) {
		if (e== userPrograms[i].environment)
			break;
	}
	if(i==NUM_USER_PROGS) 
	{
		cprintf("Unknown user program \n");
		return 0;
	}

	return &userPrograms[i];
				}

void set_environment_entry_point(struct UserProgramInfo* ptr_user_program)
{
	uint8* ptr_program_start=ptr_user_program->ptr_start;
	struct Env* e = ptr_user_program->environment;

	struct Elf * pELFHDR = (struct Elf *)ptr_program_start ; 
	if (pELFHDR->e_magic != ELF_MAGIC) 
		panic("Matafa2nash 3ala Keda"); 
	e->env_tf.tf_eip = (uint32*)pELFHDR->e_entry ;
}



//
// Frees environment e.
// If e was the current env, then runs a new environment (and does not return
// to the caller).
//
void
env_destroy(struct Env *e) 
{
	env_free(e);

	//cprintf("Destroyed the only environment - nothing more to do!\n");
	while (1)
		run_command_prompt();
}

void env_run_cmd_prmpt()
{
	struct UserProgramInfo* upi= get_user_program_info_by_env(curenv);	
	// Clear out all the saved register state,
	// to prevent the register values
	// of a prior environment inhabiting this Env structure
	// from "leaking" into our new environment.
	memset(&curenv->env_tf, 0, sizeof(curenv->env_tf));

	// Set up appropriate initial values for the segment registers.
	// GD_UD is the user data segment selector in the GDT, and 
	// GD_UT is the user text segment selector (see inc/memlayout.h).
	// The low 2 bits of each segment register contains the
	// Requestor Privilege Level (RPL); 3 means user mode.

	curenv->env_tf.tf_ds = GD_UD | 3;
	curenv->env_tf.tf_es = GD_UD | 3;
	curenv->env_tf.tf_ss = GD_UD | 3;
	curenv->env_tf.tf_esp = (uint32*)USTACKTOP;
	curenv->env_tf.tf_cs = GD_UT | 3;
	set_environment_entry_point(upi);

	lcr3(K_PHYSICAL_ADDRESS(ptr_page_directory));

	curenv = NULL;

	while (1)
		run_command_prompt();
}

//
// Restores the register values in the Trapframe with the 'iret' instruction.
// This exits the kernel and starts executing some environment's code.
// This function does not return.
//
void
env_pop_tf(struct Trapframe *tf)
{
	__asm __volatile("movl %0,%%esp\n"
			"\tpopal\n"
			"\tpopl %%es\n"
			"\tpopl %%ds\n"
			"\taddl $0x8,%%esp\n" /* skip tf_trapno and tf_errcode */
			"\tiret"
			: : "g" (tf) : "memory");
	panic("iret failed");  /* mostly to placate the compiler */
}

