// Scenario that tests the usage of shared variables
#include <inc/lib.h>

void _main(void)
{
	// Testing scenario 2: using dynamic allocation and free
	// Testing removing the allocated pages (static & dynamic) in mem, WS, mapped page tables, env's directory and env's page file

	int freeFrames_before = sys_calculate_free_frames() ;
	int usedDiskPages_before = sys_pf_calculate_allocated_pages() ;
	cprintf("\n---# of free frames before running programs = %d\n", freeFrames_before);

	/*[4] CREATE AND RUN ProcessA & ProcessB*/
	//Create 3 processes
	int32 envIdProcessA = sys_create_env("ef_ms1", 10, 50);
	int32 envIdProcessB = sys_create_env("ef_ms2", 7, 50);

	//Run 3 processes
	sys_run_env(envIdProcessA);
	sys_run_env(envIdProcessB);

	env_sleep(30000);
	cprintf("\n---# of free frames after running programs = %d\n", sys_calculate_free_frames());

	//Kill the 3 processes
	sys_free_env(envIdProcessA);
	sys_free_env(envIdProcessB);

	//Checking the number of frames after killing the created environments
	int freeFrames_after = sys_calculate_free_frames() ;
	int usedDiskPages_after = sys_pf_calculate_allocated_pages() ;

	if ((freeFrames_after - freeFrames_before) != 0) {
		cprintf("\n---# of free frames after closing running programs not as before running = %d\n",
				freeFrames_after);
		panic("env_free() does not work correctly... check it again.");
	}

	cprintf("\n---# of free frames after closing running programs returned back to be as before running = %d\n", freeFrames_after);

	cprintf("\n\nCongratulations!! test scenario 2 for envfree completed successfully.\n");
	return;
}
