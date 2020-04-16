// Scenario that tests the usage of shared variables
#include <inc/lib.h>

void _main(void)
{
	// Testing scenario 1: without using dynamic allocation/de-allocation, shared variables and semaphores
	// Testing removing the allocated pages in mem, WS, mapped page tables, env's directory and env's page file

	int freeFrames_before = sys_calculate_free_frames() ;
	int usedDiskPages_before = sys_pf_calculate_allocated_pages() ;
	cprintf("\n---# of free frames before running programs = %d\n", freeFrames_before);

	/*[4] CREATE AND RUN ProcessA & ProcessB*/
	//Create 3 processes
	int32 envIdProcessA = sys_create_env("ef_fib", 5, 50);
	int32 envIdProcessB = sys_create_env("ef_fact", 4, 50);
	int32 envIdProcessC = sys_create_env("ef_fos_add", 30, 50);

	//Run 3 processes
	sys_run_env(envIdProcessA);
	sys_run_env(envIdProcessB);
	sys_run_env(envIdProcessC);

	env_sleep(6000);
	cprintf("\n---# of free frames after running programs = %d\n", sys_calculate_free_frames());

	//Kill the 3 processes
	sys_free_env(envIdProcessA);
	sys_free_env(envIdProcessB);
	sys_free_env(envIdProcessC);

	//Checking the number of frames after killing the created environments
	int freeFrames_after = sys_calculate_free_frames() ;
	int usedDiskPages_after = sys_pf_calculate_allocated_pages() ;

	if((freeFrames_after - freeFrames_before) !=0)
		panic("env_free() does not work correctly... check it again.") ;

	cprintf("\n---# of free frames after closing running programs returned back to be as before running = %d\n", freeFrames_after);

	cprintf("\n\nCongratulations!! test scenario 1 for envfree completed successfully.\n");
	return;
}
