#include <inc/memlayout.h>
#include <kern/kheap.h>
#include <kern/memory_manager.h>
#include <inc/queue.h>
#include <kern/sched.h>

#define Mega  (1024*1024)
#define kilo (1024)

//2017
#define INITIAL_KHEAB_ALLOCATIONS (KERNEL_SHARES_ARR_INIT_SIZE + KERNEL_SEMAPHORES_ARR_INIT_SIZE + ROUNDUP(num_of_ready_queues * sizeof(uint8), PAGE_SIZE) + ROUNDUP(num_of_ready_queues * sizeof(struct Env_Queue), PAGE_SIZE))
#define ACTUAL_START (KERNEL_HEAP_START + INITIAL_KHEAB_ALLOCATIONS)

extern int pf_calculate_free_frames() ;
extern uint32 sys_calculate_free_frames() ;
extern int pf_calculate_free_frames();
extern void sys_bypassPageFault(uint8);
extern uint32 sys_rcr2();
extern int execute_command(char *command_string);

extern char end_of_kernel[];

struct MyStruct
{
	char a;
	short b;
	int c;
};


int test_kmalloc()
{
	cprintf("==============================================\n");
	cprintf("MAKE SURE to have a FRESH RUN for this test\n(i.e. don't run any program/test before it)\n");
	cprintf("==============================================\n");

	char minByte = 1<<7;
	char maxByte = 0x7F;
	short minShort = 1<<15 ;
	short maxShort = 0x7FFF;
	int minInt = 1<<31 ;
	int maxInt = 0x7FFFFFFF;

	char *byteArr, *byteArr2 ;
	short *shortArr, *shortArr2 ;
	int *intArr;
	struct MyStruct *structArr ;
	int lastIndexOfByte, lastIndexOfByte2, lastIndexOfShort, lastIndexOfShort2, lastIndexOfInt, lastIndexOfStruct;
	int start_freeFrames = sys_calculate_free_frames() ;

	void* ptr_allocations[20] = {0};
	{
		//Insufficient space
		int freeFrames = sys_calculate_free_frames() ;
		int freeDiskFrames = pf_calculate_free_frames() ;
		uint32 sizeOfKHeap = (KERNEL_HEAP_MAX - ACTUAL_START + 1) ;
		ptr_allocations[0] = kmalloc(sizeOfKHeap);
		if (ptr_allocations[0] != NULL) panic("Allocating insufficient space: should return NULL");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Wrong allocation: pages are not loaded successfully into memory");

		cprintf("\nkmalloc: current evaluation = 10%");
		//2 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[0] = kmalloc(2*Mega-kilo);
		if ((uint32) ptr_allocations[0] !=  (ACTUAL_START)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 512) panic("Wrong allocation: pages are not loaded successfully into memory");

		//2 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[1] = kmalloc(2*Mega-kilo);
		if ((uint32) ptr_allocations[1] != (ACTUAL_START + 2*Mega)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 512) panic("Wrong allocation: pages are not loaded successfully into memory");

		//2 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[2] = kmalloc(2*kilo);
		if ((uint32) ptr_allocations[2] != (ACTUAL_START + 4*Mega)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 1) panic("Wrong allocation: pages are not loaded successfully into memory");

		//2 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[3] = kmalloc(2*kilo);
		if ((uint32) ptr_allocations[3] != (ACTUAL_START + 4*Mega + 4*kilo)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 1) panic("Wrong allocation: pages are not loaded successfully into memory");

		//7 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[4] = kmalloc(7*kilo);
		if ((uint32) ptr_allocations[4] != (ACTUAL_START + 4*Mega + 8*kilo)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 2) panic("Wrong allocation: pages are not loaded successfully into memory");

		//3 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[5] = kmalloc(3*Mega-kilo);
		if ((uint32) ptr_allocations[5] != (ACTUAL_START + 4*Mega + 16*kilo) ) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 768) panic("Wrong allocation: pages are not loaded successfully into memory");

		//6 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[6] = kmalloc(6*Mega-kilo);
		if ((uint32) ptr_allocations[6] != (ACTUAL_START + 7*Mega + 16*kilo)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 1536) panic("Wrong allocation: pages are not loaded successfully into memory");

		//14 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[7] = kmalloc(14*kilo);
		if ((uint32) ptr_allocations[7] != (ACTUAL_START + 13*Mega + 16*kilo)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 4) panic("Wrong allocation: pages are not loaded successfully into memory");

		cprintf("\b\b\b50%");
		//Checking read/write on the allocated spaces
		{
			freeFrames = sys_calculate_free_frames() ;
			freeDiskFrames = pf_calculate_free_frames() ;

			//Write values
			//In 1st 2 MB
			lastIndexOfByte = (2*Mega-kilo)/sizeof(char) - 1;
			byteArr = (char *) ptr_allocations[0];
			byteArr[0] = minByte ;
			byteArr[lastIndexOfByte] = maxByte ;

			//In 2nd 2 MB
			shortArr = (short *) ptr_allocations[1];
			lastIndexOfShort = (2*Mega-kilo)/sizeof(short) - 1;
			shortArr[0] = minShort;
			shortArr[lastIndexOfShort] = maxShort;

			//In next 2 KB
			intArr = (int *) ptr_allocations[2];
			lastIndexOfInt = (2*kilo)/sizeof(int) - 1;
			intArr[0] = minInt;
			intArr[lastIndexOfInt] = maxInt;

			//In 7 KB
			structArr = (struct MyStruct *) ptr_allocations[4];
			lastIndexOfStruct = (7*kilo)/sizeof(struct MyStruct) - 1;
			structArr[0].a = minByte; structArr[0].b = minShort; structArr[0].c = minInt;
			structArr[lastIndexOfStruct].a = maxByte; structArr[lastIndexOfStruct].b = maxShort; structArr[lastIndexOfStruct].c = maxInt;

			//In 6 MB
			lastIndexOfByte2 = (6*Mega-kilo)/sizeof(char) - 1;
			byteArr2 = (char *) ptr_allocations[6];
			byteArr2[0] = minByte ;
			byteArr2[lastIndexOfByte2 / 2] = maxByte / 2;
			byteArr2[lastIndexOfByte2] = maxByte ;

			//In 14 KB
			shortArr2 = (short *) ptr_allocations[7];
			lastIndexOfShort2 = (14*kilo)/sizeof(short) - 1;
			shortArr2[0] = minShort;
			shortArr2[lastIndexOfShort2] = maxShort;

			//Read values: check that the values are successfully written
			if (byteArr[0] 	!= minByte 	|| byteArr[lastIndexOfByte] 	!= maxByte) panic("Wrong allocation: stored values are wrongly changed!");
			if (shortArr[0] != minShort || shortArr[lastIndexOfShort] 	!= maxShort) panic("Wrong allocation: stored values are wrongly changed!");
			if (intArr[0] 	!= minInt 	|| intArr[lastIndexOfInt] 		!= maxInt) panic("Wrong allocation: stored values are wrongly changed!");

			if (structArr[0].a != minByte 	|| structArr[lastIndexOfStruct].a != maxByte) 	panic("Wrong allocation: stored values are wrongly changed!");
			if (structArr[0].b != minShort 	|| structArr[lastIndexOfStruct].b != maxShort) 	panic("Wrong allocation: stored values are wrongly changed!");
			if (structArr[0].c != minInt 	|| structArr[lastIndexOfStruct].c != maxInt) 	panic("Wrong allocation: stored values are wrongly changed!");

			if (byteArr2[0] != minByte || byteArr2[lastIndexOfByte2/2] != maxByte/2 || byteArr2[lastIndexOfByte2] != maxByte) panic("Wrong allocation: stored values are wrongly changed!");
			if (shortArr2[0] != minShort || shortArr2[lastIndexOfShort2] != maxShort) panic("Wrong allocation: stored values are wrongly changed!");

			if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Wrong allocation: pages are not loaded successfully into memory");
			if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		}

		cprintf("\b\b\b80%");

		//Insufficient space again
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		uint32 restOfKHeap = (KERNEL_HEAP_MAX - ACTUAL_START + 2*PAGE_SIZE) - (2*Mega+2*Mega+4*kilo+4*kilo+8*kilo+3*Mega+6*Mega+16*kilo) ;
		ptr_allocations[8] = kmalloc(restOfKHeap);
		if (ptr_allocations[8] != NULL) panic("Allocating insufficient space: should return NULL");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Wrong allocation: pages are not loaded successfully into memory");

		cprintf("\b\b\b90%");

		//permissions
		uint32 lastAllocAddress = (uint32)ptr_allocations[7] + 16*kilo ;
		uint32 va;
		for (va = ACTUAL_START; va < lastAllocAddress; va+=PAGE_SIZE)
		{
			unsigned int * table;
			get_page_table(ptr_page_directory, (void*)va, &table);
			uint32 perm = table[PTX(va)] & 0xFFF;
			if ((perm & PERM_USER) == PERM_USER)
				panic("Wrong permissions: pages should be mapped with Supervisor permission only");
		}

		cprintf("\b\b\b100%\n");

	}

	cprintf("\nCongratulations!! test kmalloc completed successfully.\n");

	return 1;

}

int test_kmalloc_nextfit()
{
	cprintf("==============================================\n");
	cprintf("MAKE SURE to have a FRESH RUN for this test\n(i.e. don't run any program/test before it)\n");
	cprintf("==============================================\n");

	void* ptr_allocations[160] = {0};
	cprintf("This test has THREE cases. A pass message will be displayed after each one.\n");

	// allocate pages
	int freeFrames = sys_calculate_free_frames() ;
	int freeDiskFrames = pf_calculate_free_frames() ;

	int i;
	//ptr_allocations[0] = kmalloc(2*Mega - KERNEL_SHARES_ARR_INIT_SIZE - KERNEL_SEMAPHORES_ARR_INIT_SIZE);
	for(i = 0; i< 79 ;i++)
	{
		ptr_allocations[i] = kmalloc(2*Mega);
	}
	ptr_allocations[79] = kmalloc(2*Mega - PAGE_SIZE - INITIAL_KHEAB_ALLOCATIONS);


	// randomly check the addresses of the allocation
	if( 	(uint32)ptr_allocations[0] != ACTUAL_START ||
			(uint32)ptr_allocations[2] != (ACTUAL_START + 4*Mega) ||
			(uint32)ptr_allocations[8] != (ACTUAL_START + 16*Mega) ||
			(uint32)ptr_allocations[10] != (ACTUAL_START + 20*Mega) ||
			(uint32)ptr_allocations[15] != (ACTUAL_START + 30*Mega) ||
			(uint32)ptr_allocations[20] != (ACTUAL_START + 40*Mega) ||
			(uint32)ptr_allocations[25] != (ACTUAL_START + 50*Mega) ||
			(uint32)ptr_allocations[79] != (ACTUAL_START + 158*Mega ))
		panic("Wrong allocation, Check next fitting strategy is working correctly");

	if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
	if ((freeFrames - sys_calculate_free_frames()) != (160*Mega - PAGE_SIZE - INITIAL_KHEAB_ALLOCATIONS)/(PAGE_SIZE) ) panic("Wrong allocation");

	// Make memory holes.
	freeDiskFrames = pf_calculate_free_frames() ;
	freeFrames = sys_calculate_free_frames() ;

	kfree(ptr_allocations[0]);		// Hole 1 = 2 M
	kfree(ptr_allocations[2]);		// Hole 2 = 4 M
	kfree(ptr_allocations[3]);
	kfree(ptr_allocations[5]);		// Hole 3 = 2 M
	kfree(ptr_allocations[10]);		// Hole 4 = 6 M
	kfree(ptr_allocations[12]);
	kfree(ptr_allocations[11]);
	kfree(ptr_allocations[20]);		// Hole 5 = 2 M
	kfree(ptr_allocations[25]);		// Hole 6 = 2 M
	kfree(ptr_allocations[79]);		// Hole 7 = 2 M - 4 KB

	if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
	if ((sys_calculate_free_frames() - freeFrames) != ((10*2*Mega) - PAGE_SIZE - INITIAL_KHEAB_ALLOCATIONS)/PAGE_SIZE) panic("Wrong free: Extra or less pages are removed from main memory");

	// Test next fit
	freeDiskFrames = pf_calculate_free_frames() ;
	freeFrames = sys_calculate_free_frames() ;
	void* tempAddress = kmalloc(Mega-kilo);		// Use Hole 1 -> Hole 1 = 1 M
	if((uint32)tempAddress != ACTUAL_START)
		panic("Next Fit not working correctly");
	if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
	if ((freeFrames - sys_calculate_free_frames()) != (1*Mega)/PAGE_SIZE) panic("Wrong allocation");

	freeDiskFrames = pf_calculate_free_frames() ;
	freeFrames = sys_calculate_free_frames() ;
	tempAddress = kmalloc(kilo);					// Use Hole 1 -> Hole 1 = 1 M - Kilo -> requires one page only
	if((uint32)tempAddress != ACTUAL_START + 0x00100000)
		panic("Next Fit not working correctly");
	if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
	if ((freeFrames - sys_calculate_free_frames()) != 1) panic("Wrong allocation");

	freeDiskFrames = pf_calculate_free_frames() ;
	freeFrames = sys_calculate_free_frames() ;
	tempAddress = kmalloc(5*Mega); 			   // Use Hole 4 -> Hole 4 = 1 M
	if((uint32)tempAddress != ACTUAL_START + 0x01400000)
		panic("Next Fit not working correctly");
	if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
	if ((freeFrames - sys_calculate_free_frames()) != (5*Mega)/PAGE_SIZE) panic("Wrong allocation");

	freeDiskFrames = pf_calculate_free_frames() ;
	freeFrames = sys_calculate_free_frames() ;
	tempAddress = kmalloc(1*Mega); 			   // Use Hole 4 -> Hole 4 = 0 M
	if((uint32)tempAddress != ACTUAL_START + 0x01900000)
		panic("Next Fit not working correctly");
	if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
	if ((freeFrames - sys_calculate_free_frames()) != (1*Mega)/PAGE_SIZE) panic("Wrong allocation");

	freeDiskFrames = pf_calculate_free_frames() ;
	freeFrames = sys_calculate_free_frames() ;
	kfree(ptr_allocations[15]);					// Make a new hole => 2 M
	if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
	if ((sys_calculate_free_frames() - freeFrames) !=  (2*Mega)/PAGE_SIZE) panic("Wrong free: Extra or less pages are removed from main memory");

	freeDiskFrames = pf_calculate_free_frames() ;
	freeFrames = sys_calculate_free_frames() ;
	tempAddress = kmalloc(kilo); 			   // Use new Hole = 2 M - 4 kilo
	if((uint32)tempAddress != ACTUAL_START + 0x01E00000)
		panic("Next Fit not working correctly");
	if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
	if ((freeFrames - sys_calculate_free_frames()) != 1) panic("Wrong allocation");

	freeDiskFrames = pf_calculate_free_frames() ;
	freeFrames = sys_calculate_free_frames() ;
	tempAddress = kmalloc(Mega + 1016*kilo); 	// Use new Hole = 4 kilo
	if((uint32)tempAddress != ACTUAL_START + 0x01E01000)
		panic("Next Fit not working correctly");
	if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");

	if ((freeFrames - sys_calculate_free_frames()) != (1*Mega+1016*kilo)/PAGE_SIZE) panic("Wrong allocation");

	freeDiskFrames = pf_calculate_free_frames() ;
	freeFrames = sys_calculate_free_frames() ;
	tempAddress = kmalloc(512*kilo); 			   // Use Hole 5 -> Hole 5 = 1.5 M
	if((uint32)tempAddress != ACTUAL_START + 0x02800000)
		panic("Next Fit not working correctly");
	if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
	if ((freeFrames - sys_calculate_free_frames()) != (512*kilo)/PAGE_SIZE) panic("Wrong allocation");

	cprintf("\nCASE1: (next fit without looping back) is succeeded...\n") ;
	/******************************/

	// Check that next fit is looping back to check for free space
	freeDiskFrames = pf_calculate_free_frames() ;
	freeFrames = sys_calculate_free_frames() ;
	tempAddress = kmalloc(3*Mega + 512*kilo); 			   // Use Hole 2 -> Hole 2 = 0.5 M
	if((uint32)tempAddress != ACTUAL_START + 0x00400000)
		panic("Next Fit not working correctly");
	if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
	if ((freeFrames - sys_calculate_free_frames()) != (3*Mega+512*kilo)/PAGE_SIZE) panic("Wrong allocation");

	freeDiskFrames = pf_calculate_free_frames() ;
	freeFrames = sys_calculate_free_frames() ;
	kfree(ptr_allocations[24]);		// Increase size of Hole 6 to 4 M
	if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
	if ((sys_calculate_free_frames() - freeFrames) != (2*Mega)/PAGE_SIZE) panic("Wrong free: Extra or less pages are removed from main memory");

	freeDiskFrames = pf_calculate_free_frames() ;
	freeFrames = sys_calculate_free_frames() ;
	tempAddress = kmalloc(4*Mega-kilo);		// Use Hole 6 -> Hole 6 = 0 M
	if((uint32)tempAddress != ACTUAL_START + 0x03000000)
		panic("Next Fit not working correctly");
	if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
	if ((freeFrames - sys_calculate_free_frames()) != (4*Mega)/PAGE_SIZE) panic("Wrong allocation");

	cprintf("\nCASE2: (next fit WITH looping back) is succeeded...\n") ;
	/******************************/

	// Check that next fit returns null in case all holes are not free
	freeDiskFrames = pf_calculate_free_frames() ;
	freeFrames = sys_calculate_free_frames() ;
	tempAddress = kmalloc(6*Mega); 			   // No Suitable Hole is available
	if((uint32)tempAddress != 0x0)
		panic("Next Fit not working correctly");
	if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
	if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Wrong allocation");

	cprintf("\nCASE3: (next fit with insufficient space) is succeeded...\n") ;
	/******************************/

	cprintf("Congratulations!! test Next Fit completed successfully.\n");
	return 1;

}

int test_kmalloc_bestfit1()
{
	cprintf("==============================================\n");
	cprintf("MAKE SURE to have a FRESH RUN for this test\n(i.e. don't run any program/test before it)\n");
	cprintf("==============================================\n");

	void* ptr_allocations[20] = {0};
	uint32 freeFrames;
	uint32 freeDiskFrames;

	//[1] Allocate all
	{
		//Allocate 3 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[0] = kmalloc(3*Mega-kilo);
		if ((uint32) ptr_allocations[0] != (ACTUAL_START)) panic("Wrong start address for the allocated space... ");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != ((3*Mega)/PAGE_SIZE)) panic("Wrong allocation: ");

		//Allocate 3 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[1] = kmalloc(3*Mega-kilo);
		if ((uint32) ptr_allocations[1] !=  (ACTUAL_START + 3*Mega)) panic("Wrong start address for the allocated space... ");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != ((3*Mega)/PAGE_SIZE)) panic("Wrong allocation: ");

		//Allocate 2 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[2] = kmalloc(2*Mega-kilo);
		if ((uint32) ptr_allocations[2] !=  (ACTUAL_START + 6*Mega)) panic("Wrong start address for the allocated space... ");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != ((2*Mega)/PAGE_SIZE)) panic("Wrong allocation: ");

		//Allocate 2 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[3] = kmalloc(2*Mega-kilo);
		if ((uint32) ptr_allocations[3] != (ACTUAL_START + 8*Mega)) panic("Wrong start address for the allocated space... ");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) !=  ((2*Mega)/PAGE_SIZE)) panic("Wrong allocation: ");

		//Allocate 1 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[4] = kmalloc(1*Mega-kilo);
		if ((uint32) ptr_allocations[4] !=  (ACTUAL_START + 10*Mega)) panic("Wrong start address for the allocated space... ");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 256) panic("Wrong allocation: ");

		//Allocate 1 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[5] = kmalloc(1*Mega-kilo);
		if ((uint32) ptr_allocations[5] != (ACTUAL_START + 11*Mega)) panic("Wrong start address for the allocated space... ");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 256) panic("Wrong allocation: ");

		//Allocate 1 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[6] = kmalloc(1*Mega-kilo);
		if ((uint32) ptr_allocations[6] != (ACTUAL_START + 12*Mega)) panic("Wrong start address for the allocated space... ");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 256) panic("Wrong allocation: ");

		//Allocate 1 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[7] = kmalloc(1*Mega-kilo);
		if ((uint32) ptr_allocations[7] != (ACTUAL_START + 13*Mega)) panic("Wrong start address for the allocated space... ");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 256) panic("Wrong allocation: ");
	}

	//[2] Free some to create holes
	{
		//3 MB Hole
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[1]);
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != ((3*Mega)/PAGE_SIZE)) panic("Wrong free: ");

		//2 MB Hole
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[3]);
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != ((2*Mega)/PAGE_SIZE)) panic("Wrong free: ");

		//1 MB Hole
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[5]);
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 256) panic("Wrong free: ");
	}

	//[3] Allocate again [test best fit]
	{
		//Allocate 512 KB - should be placed in 3rd hole
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[8] = kmalloc(512*kilo);
		if ((uint32) ptr_allocations[8] !=  (ACTUAL_START + 11*Mega)) panic("Wrong start address for the allocated space... ");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 128) panic("Wrong allocation: ");

		//Allocate 1 MB - should be placed in 2nd hole
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[9] = kmalloc(1*Mega - kilo);
		if ((uint32) ptr_allocations[9] !=  (ACTUAL_START + 8*Mega)) panic("Wrong start address for the allocated space... ");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 256) panic("Wrong allocation: ");

		//Allocate 256 KB - should be placed in remaining of 3rd hole
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[10] = kmalloc(256*kilo - kilo);
		if ((uint32) ptr_allocations[10] !=  (ACTUAL_START + 11*Mega + 512*kilo)) panic("Wrong start address for the allocated space... ");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 64) panic("Wrong allocation: ");

		//Allocate 4 MB - should be placed in end of all allocations
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[11] = kmalloc(4*Mega - kilo);
		if ((uint32) ptr_allocations[11] != (ACTUAL_START + 14*Mega)) panic("Wrong start address for the allocated space... ");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 1024) panic("Wrong allocation: ");
	}

	//[4] Free contiguous allocations
	{
		//1M Hole appended to already existing 1M hole in the middle
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[4]);
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 256) panic("Wrong free: ");

		//another 512 KB Hole appended to the hole
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[8]);
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 128) panic("Wrong free: ");
	}

	//[5] Allocate again [test best fit]
	{
		//Allocate 2 MB - should be placed in the contiguous hole (2 MB + 512 KB)
		freeFrames = sys_calculate_free_frames();
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[12] = kmalloc(2*Mega - kilo);
		if ((uint32) ptr_allocations[12] != (ACTUAL_START + 9*Mega)) panic("Wrong start address for the allocated space... ");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 512) panic("Wrong allocation: ");
	}

	cprintf("Congratulations!! test BEST FIT allocation (1) completed successfully.\n");

	return 1;

}

int test_kmalloc_bestfit2()
{
	cprintf("==============================================\n");
	cprintf("MAKE SURE to have a FRESH RUN for this test\n(i.e. don't run any program/test before it)\n");
	cprintf("==============================================\n");

	void* ptr_allocations[20] = {0};
	uint32 freeFrames;
	uint32 freeDiskFrames;

	//[1] Attempt to allocate more than heap size
	{
		ptr_allocations[0] = kmalloc(KERNEL_HEAP_MAX - ACTUAL_START + 1);
		if (ptr_allocations[0] != NULL) panic("Kmalloc: Attempt to allocate more than heap size, should return NULL");
	}

	//[2] Attempt to allocate space more than any available fragment
	//	a) Create Fragments
	{
		//2 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames();
		ptr_allocations[0] = kmalloc(2*Mega-kilo);
		if ((uint32) ptr_allocations[0] != (ACTUAL_START)) panic("Wrong start address for the allocated space... ");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) !=  512) panic("Wrong allocation: ");

		//2 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames();
		ptr_allocations[1] = kmalloc(2*Mega-kilo);
		if ((uint32) ptr_allocations[1] != (ACTUAL_START + 2*Mega)) panic("Wrong start address for the allocated space... ");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) !=  512) panic("Wrong allocation: ");

		//2 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames();
		ptr_allocations[2] = kmalloc(2*kilo);
		if ((uint32) ptr_allocations[2] != (ACTUAL_START + 4*Mega)) panic("Wrong start address for the allocated space... ");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) !=  1) panic("Wrong allocation: ");

		//2 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames();
		ptr_allocations[3] = kmalloc(2*kilo);
		if ((uint32) ptr_allocations[3] != (ACTUAL_START + 4*Mega + 4*kilo)) panic("Wrong start address for the allocated space... ");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) !=  1) panic("Wrong allocation: ");

		//4 KB Hole
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames();
		kfree(ptr_allocations[2]);
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 1) panic("Wrong allocation: ");

		//7 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames();
		ptr_allocations[4] = kmalloc(7*kilo);
		if ((uint32) ptr_allocations[4] != (ACTUAL_START + 4*Mega + 8*kilo)) panic("Wrong start address for the allocated space... ");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) !=  2) panic("Wrong allocation: ");

		//2 MB Hole
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames();
		kfree(ptr_allocations[0]);
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 512) panic("Wrong free: Extra or less pages are removed from main memory");

		//3 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames();
		ptr_allocations[5] = kmalloc(3*Mega-kilo);
		if ((uint32) ptr_allocations[5] != (ACTUAL_START + 4*Mega + 16*kilo)) panic("Wrong start address for the allocated space... ");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) !=  768) panic("Wrong allocation: ");

		//2 MB + 6 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames();
		ptr_allocations[6] = kmalloc(2*Mega + 6*kilo);
		if ((uint32) ptr_allocations[6] != (ACTUAL_START + 7*Mega + 16*kilo)) panic("Wrong start address for the allocated space... ");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) !=  514) panic("Wrong allocation: ");

		//5 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames();
		ptr_allocations[7] = kmalloc(5*Mega-kilo);
		if ((uint32) ptr_allocations[7] != (ACTUAL_START + 9*Mega + 24*kilo)) panic("Wrong start address for the allocated space... ");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) !=  ((5*Mega)/PAGE_SIZE)) panic("Wrong allocation: ");

		//2 MB + 8 KB Hole
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames();
		kfree(ptr_allocations[6]);
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) !=  514) panic("Wrong free: Extra or less pages are removed from main memory");

		//2 MB Hole
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames();
		kfree(ptr_allocations[1]);
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) !=  512) panic("Wrong free: Extra or less pages are removed from main memory.");

		//2 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames();
		ptr_allocations[8] = kmalloc(2*Mega-kilo);
		if ((uint32) ptr_allocations[8] != (ACTUAL_START + 7*Mega + 16*kilo)) panic("Wrong start address for the allocated space... ");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) !=  512) panic("Wrong allocation:");

		//6 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames();
		ptr_allocations[9] = kmalloc(6*kilo);
		if ((uint32) ptr_allocations[9] != (ACTUAL_START + 9*Mega + 16*kilo)) panic("Wrong start address for the allocated space... ");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) !=  2) panic("Wrong allocation:");

		//3 MB Hole
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames();
		kfree(ptr_allocations[5]);
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) !=  768) panic("Wrong free: Extra or less pages are removed from main memory.");

		//3 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames();
		ptr_allocations[10] = kmalloc(3*Mega-kilo);
		if ((uint32) ptr_allocations[10] != (ACTUAL_START + 4*Mega + 16*kilo)) panic("Wrong start address for the allocated space... ");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) !=  ((3*Mega)/4096)) panic("Wrong free: Extra or less pages are removed from main memory.");

		//4 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames();
		ptr_allocations[11] = kmalloc(4*Mega-kilo);
		if ((uint32) ptr_allocations[11] != (ACTUAL_START)) panic("Wrong start address for the allocated space... ");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != ((4*Mega)/4096)) panic("Wrong free: Extra or less pages are removed from main memory.");

	}

	//	b) Attempt to allocate large segment with no suitable fragment to fit on
	{
		//Large Allocation
		ptr_allocations[12] = kmalloc((KERNEL_HEAP_MAX - ACTUAL_START - 14*Mega));
		if (ptr_allocations[12] != NULL) panic("Kmalloc: Attempt to allocate large segment with no suitable fragment to fit on, should return NULL");

		cprintf("Congratulations!! test BEST FIT allocation (2) completed successfully.\n");
	}
	return 1;

}

int test_kmalloc_firstfit1()
{
	cprintf("==============================================\n");
	cprintf("MAKE SURE to have a FRESH RUN for this test\n(i.e. don't run any program/test before it)\n");
	cprintf("==============================================\n");

	void* ptr_allocations[20] = {0};
	uint32 freeFrames;
	uint32 freeDiskFrames;

	//[1] Allocate all
	{
		//Allocate 1 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[0] = kmalloc(1*Mega-kilo);
		if ((uint32) ptr_allocations[0] != (ACTUAL_START)) panic("Wrong start address for the allocated space... ");
		if((pf_calculate_free_frames() - freeDiskFrames) !=  0)  panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 256) panic("Wrong allocation: ");

		//Allocate 1 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[1] = kmalloc(1*Mega-kilo);
		if ((uint32) ptr_allocations[1] != (ACTUAL_START + 1*Mega)) panic("Wrong start address for the allocated space... ");
		if((pf_calculate_free_frames() - freeDiskFrames) !=  0)  panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 256) panic("Wrong allocation: ");

		//Allocate 1 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[2] = kmalloc(1*Mega-kilo);
		if ((uint32) ptr_allocations[2] != (ACTUAL_START + 2*Mega)) panic("Wrong start address for the allocated space... ");
		if((pf_calculate_free_frames() - freeDiskFrames) !=  0)  panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 256) panic("Wrong allocation: ");

		//Allocate 1 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[3] = kmalloc(1*Mega-kilo);
		if ((uint32) ptr_allocations[3] != (ACTUAL_START + 3*Mega)) panic("Wrong start address for the allocated space... ");
		if((pf_calculate_free_frames() - freeDiskFrames) !=  0)  panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 256) panic("Wrong allocation: ");

		//Allocate 2 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[4] = kmalloc(2*Mega-kilo);
		if ((uint32) ptr_allocations[4] != (ACTUAL_START + 4*Mega)) panic("Wrong start address for the allocated space... ");
		if((pf_calculate_free_frames() - freeDiskFrames) !=  0)  panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 512) panic("Wrong allocation: ");

		//Allocate 2 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[5] = kmalloc(2*Mega-kilo);
		if ((uint32) ptr_allocations[5] != (ACTUAL_START + 6*Mega)) panic("Wrong start address for the allocated space... ");
		if((pf_calculate_free_frames() - freeDiskFrames) !=  0)  panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 512) panic("Wrong allocation: ");

		//Allocate 3 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[6] = kmalloc(3*Mega-kilo);
		if ((uint32) ptr_allocations[6] !=  (ACTUAL_START + 8*Mega)) panic("Wrong start address for the allocated space... ");
		if((pf_calculate_free_frames() - freeDiskFrames) !=  0)  panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 768) panic("Wrong allocation: ");

		//Allocate 3 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[7] = kmalloc(3*Mega-kilo);
		if ((uint32) ptr_allocations[7] != (ACTUAL_START + 11*Mega)) panic("Wrong start address for the allocated space... ");
		if((pf_calculate_free_frames() - freeDiskFrames) !=  0)  panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 768) panic("Wrong allocation: ");
	}

	//[2] Free some to create holes
	{
		//1 MB Hole
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[1]);
		if((pf_calculate_free_frames() - freeDiskFrames) !=  0)  panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 256) panic("Wrong free: ");

		//2 MB Hole
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[4]);
		if((pf_calculate_free_frames() - freeDiskFrames) !=  0)  panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 512) panic("Wrong free: ");

		//3 MB Hole
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[6]);
		if((pf_calculate_free_frames() - freeDiskFrames) !=  0)  panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 768) panic("Wrong free: ");
	}

	//[3] Allocate again [test first fit]
	{
		//Allocate 512 KB - should be placed in 1st hole
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[8] = kmalloc(512*kilo - kilo);
		if ((uint32) ptr_allocations[8] != (ACTUAL_START + 1*Mega)) panic("Wrong start address for the allocated space... ");
		if((pf_calculate_free_frames() - freeDiskFrames) !=  0)  panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 128) panic("Wrong allocation: ");

		//Allocate 1 MB - should be placed in 2nd hole
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[9] = kmalloc(1*Mega - kilo);
		if ((uint32) ptr_allocations[9] != (ACTUAL_START + 4*Mega)) panic("Wrong start address for the allocated space... ");
		if((pf_calculate_free_frames() - freeDiskFrames) !=  0)  panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 256) panic("Wrong allocation: ");


		//Allocate 256 KB - should be placed in remaining of 1st hole
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[10] = kmalloc(256*kilo - kilo);
		if ((uint32) ptr_allocations[10] != (ACTUAL_START + 1*Mega + 512*kilo)) panic("Wrong start address for the allocated space... ");
		if((pf_calculate_free_frames() - freeDiskFrames) !=  0)  panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 64) panic("Wrong allocation: ");

		//Allocate 2 MB - should be placed in 3rd hole
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[11] = kmalloc(2*Mega);
		if ((uint32) ptr_allocations[11] != (ACTUAL_START + 8*Mega)) panic("Wrong start address for the allocated space... ");
		if ((freeFrames - sys_calculate_free_frames()) != 512) panic("Wrong allocation: ");
		if((pf_calculate_free_frames() - freeDiskFrames) !=  0)  panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");


		//Allocate 4 MB - should be placed in end of all allocations
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[12] = kmalloc(4*Mega - kilo);
		if ((uint32) ptr_allocations[12] != (ACTUAL_START + 14*Mega)) panic("Wrong start address for the allocated space... ");
		if((pf_calculate_free_frames() - freeDiskFrames) !=  0)  panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 1024) panic("Wrong allocation: ");
	}

	//[4] Free contiguous allocations
	{
		//1 MB Hole appended to previous 256 KB hole
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[2]);
		if((pf_calculate_free_frames() - freeDiskFrames) !=  0)  panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 256) panic("Wrong free: ");

		//Next 1 MB Hole appended also
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[3]);
		if((pf_calculate_free_frames() - freeDiskFrames) !=  0)  panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 256) panic("Wrong free: ");
	}

	//[5] Allocate again [test first fit]
	{
		//[FIRST FIT Case]
		//Allocate 1 MB - should be placed in the contiguous hole (256 KB + 2 MB)
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[13] = kmalloc(1*Mega);
		if ((uint32) ptr_allocations[13] != (ACTUAL_START + 1*Mega + 768*kilo)) panic("Wrong start address for the allocated space... ");
		if((pf_calculate_free_frames() - freeDiskFrames) !=  0)  panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 256) panic("Wrong allocation: ");
	}

	cprintf("Congratulations!! test FIRST FIT allocation (1) completed successfully.\n");

	return 1;
}

int test_kmalloc_firstfit2()
{
	cprintf("==============================================\n");
	cprintf("MAKE SURE to have a FRESH RUN for this test\n(i.e. don't run any program/test before it)\n");
	cprintf("==============================================\n");

	void* ptr_allocations[20] = {0};
	uint32 freeFrames;
	uint32 freeDiskFrames;

	//[1] Attempt to allocate more than heap size
	{
		ptr_allocations[0] = kmalloc(KERNEL_HEAP_MAX - ACTUAL_START + 1);
		if (ptr_allocations[0] != NULL) panic("kmalloc: Attempt to allocate more than heap size, should return NULL");
	}

	//[2] Attempt to allocate space more than any available fragment
	//	a) Create Fragments
	{
		//2 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[0] = kmalloc(2*Mega-kilo);
		if ((uint32) ptr_allocations[0] != (ACTUAL_START)) panic("Wrong start address for the allocated space... ");
		if((pf_calculate_free_frames() - freeDiskFrames) !=  0)  panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 512) panic("Wrong allocation: ");

		//2 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[1] = kmalloc(2*Mega-kilo);
		if ((uint32) ptr_allocations[1] != (ACTUAL_START + 2*Mega)) panic("Wrong start address for the allocated space... ");
		if((pf_calculate_free_frames() - freeDiskFrames) !=  0)  panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 512) panic("Wrong allocation: ");

		//2 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[2] = kmalloc(2*kilo);
		if ((uint32) ptr_allocations[2] < (ACTUAL_START + 4*Mega)) panic("Wrong start address for the allocated space... ");
		if((pf_calculate_free_frames() - freeDiskFrames) !=  0)  panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 1) panic("Wrong allocation: ");

		//2 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[3] = kmalloc(2*kilo);
		if ((uint32) ptr_allocations[3] != (ACTUAL_START + 4*Mega + 4*kilo)) panic("Wrong start address for the allocated space... ");
		if((pf_calculate_free_frames() - freeDiskFrames) !=  0)  panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 1) panic("Wrong allocation: ");

		//4 KB Hole
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[2]);
		if((pf_calculate_free_frames() - freeDiskFrames) !=  0)  panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 1) panic("Wrong free: ");

		//7 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[4] = kmalloc(7*kilo);
		if ((uint32) ptr_allocations[4] != (ACTUAL_START + 4*Mega + 8*kilo)) panic("Wrong start address for the allocated space... ");
		if((pf_calculate_free_frames() - freeDiskFrames) !=  0)  panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 2) panic("Wrong allocation: ");

		//2 MB Hole
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[0]);
		if((pf_calculate_free_frames() - freeDiskFrames) !=  0)  panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 512) panic("Wrong free: ");

		//3 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[5] = kmalloc(3*Mega-kilo);
		if ((uint32) ptr_allocations[5] != (ACTUAL_START + 4*Mega + 16*kilo)) panic("Wrong start address for the allocated space... ");
		if((pf_calculate_free_frames() - freeDiskFrames) !=  0)  panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) !=  3*Mega/PAGE_SIZE) panic("Wrong allocation: ");

		//2 MB + 6 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[6] = kmalloc(2*Mega + 6*kilo);
		if ((uint32) ptr_allocations[6] != (ACTUAL_START + 7*Mega + 16*kilo)) panic("Wrong start address for the allocated space... ");
		if((pf_calculate_free_frames() - freeDiskFrames) !=  0)  panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) !=  514) panic("Wrong allocation: ");

		//3 MB Hole
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[5]);
		if((pf_calculate_free_frames() - freeDiskFrames) !=  0)  panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 768) panic("Wrong free: ");

		//2 MB Hole [Resulting Hole = 2 MB + 2 MB + 4 KB = 4 MB + 4 KB]
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[1]);
		if ((sys_calculate_free_frames() - freeFrames) != 512) panic("Wrong free: ");
		if((pf_calculate_free_frames() - freeDiskFrames) !=  0)  panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");

		//5 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[7] = kmalloc(5*Mega-kilo);
		if ((uint32) ptr_allocations[7] != (ACTUAL_START + 9*Mega + 24*kilo)) panic("Wrong start address for the allocated space... ");
		if((pf_calculate_free_frames() - freeDiskFrames) !=  0)  panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) !=   5*Mega/PAGE_SIZE) panic("Wrong allocation: ");

//		//2 MB + 8 KB Hole
//		freeFrames = sys_calculate_free_frames() ;
//		freeDiskFrames = pf_calculate_free_frames() ;
//		kfree(ptr_allocations[6]);
//		if((pf_calculate_free_frames() - freeDiskFrames) !=  0)  panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
//		if ((sys_calculate_free_frames() - freeFrames) != 514) panic("Wrong free: ");

		//[FIRST FIT Case]
		//3 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[8] = kmalloc(3*Mega-kilo);
		if ((uint32) ptr_allocations[8] != (ACTUAL_START)) panic("Wrong start address for the allocated space... ");
		if((freeDiskFrames - pf_calculate_free_frames()) !=  0)  panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) !=   3*Mega/PAGE_SIZE) panic("Wrong allocation: ");
	}
	//	b) Attempt to allocate large segment with no suitable fragment to fit on
	{
		//Large Allocation
		ptr_allocations[9] = kmalloc((KERNEL_HEAP_MAX - ACTUAL_START - 14*Mega));
		if (ptr_allocations[9] != NULL) panic("Kmalloc: Attempt to allocate large segment with no suitable fragment to fit on, should return NULL");

		cprintf("Congratulations!! test FIRST FIT allocation (2) completed successfully.\n");
	}
	return 1;
}

int test_kmalloc_worstfit()
{
	cprintf("==============================================\n");
	cprintf("MAKE SURE to have a FRESH RUN for this test\n(i.e. don't run any program/test before it)\n");
	cprintf("==============================================\n");

	void* ptr_allocations[160] = {0};

	// allocate pages
	int freeFrames = sys_calculate_free_frames() ;
	int freeDiskFrames = pf_calculate_free_frames() ;

	int count = 0;
	int i;
	for(i = 0; i< 79 ;i++)
	{
		ptr_allocations[i] = kmalloc(2*Mega);
	}
	ptr_allocations[79] = kmalloc(2*Mega - PAGE_SIZE - KERNEL_SHARES_ARR_INIT_SIZE - KERNEL_SEMAPHORES_ARR_INIT_SIZE);

	// randomly check the addresses of the allocation
	if( 	(uint32)ptr_allocations[0] != ACTUAL_START ||
			(uint32)ptr_allocations[2] != (ACTUAL_START + 4*Mega) ||
			(uint32)ptr_allocations[8] != (ACTUAL_START + 16*Mega) ||
			(uint32)ptr_allocations[10] != (ACTUAL_START + 20*Mega) ||
			(uint32)ptr_allocations[15] != (ACTUAL_START + 30*Mega) ||
			(uint32)ptr_allocations[20] != (ACTUAL_START + 40*Mega) ||
			(uint32)ptr_allocations[50] != (ACTUAL_START + 100*Mega) ||
			(uint32)ptr_allocations[79] != (ACTUAL_START + 158*Mega))
		panic("Wrong allocation, Check next fitting strategy is working correctly");

	if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
	if ((freeFrames - sys_calculate_free_frames()) != (160*Mega - PAGE_SIZE - KERNEL_SHARES_ARR_INIT_SIZE - KERNEL_SEMAPHORES_ARR_INIT_SIZE)/(PAGE_SIZE) ) panic("Wrong allocation");

	//make memory holes
	freeFrames = sys_calculate_free_frames() ;
	freeDiskFrames = pf_calculate_free_frames() ;

	kfree(ptr_allocations[0]);		//Hole 1 = 2 M
	kfree(ptr_allocations[2]);		//Hole 2 = 4 M
	kfree(ptr_allocations[3]);
	kfree(ptr_allocations[10]);		//Hole 3 = 6 M
	kfree(ptr_allocations[12]);
	kfree(ptr_allocations[11]);
	kfree(ptr_allocations[30]);		//Hole 4 = 10 M
	kfree(ptr_allocations[31]);
	kfree(ptr_allocations[32]);
	kfree(ptr_allocations[33]);
	kfree(ptr_allocations[34]);
	kfree(ptr_allocations[70]); 	//Hole 5 = 8 M
	kfree(ptr_allocations[71]);
	kfree(ptr_allocations[72]);
	kfree(ptr_allocations[73]);

	if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
	if ((sys_calculate_free_frames() - freeFrames) != ((15*2*Mega))/PAGE_SIZE) panic("Wrong free: Extra or less pages are removed from main memory");

	// Test worst fit
	freeFrames = sys_calculate_free_frames() ;
	freeDiskFrames = pf_calculate_free_frames();
	void* tempAddress = kmalloc(Mega);		//Use Hole 4 -> Hole 4 = 9 M
	if((uint32)tempAddress != ACTUAL_START + 0x03C00000)
		panic("Worst Fit not working correctly");
	if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
	if ((freeFrames - sys_calculate_free_frames()) !=  1*Mega/PAGE_SIZE) panic("Wrong allocation:");
	cprintf("Test %d Passed \n", ++count);

	freeFrames = sys_calculate_free_frames() ;
	freeDiskFrames = pf_calculate_free_frames();
	tempAddress = kmalloc(4 * Mega);			//Use Hole 4 -> Hole 4 = 5 M
	if((uint32)tempAddress != ACTUAL_START + 0x03D00000)
		panic("Worst Fit not working correctly");
	if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
	if ((freeFrames - sys_calculate_free_frames()) !=  4*Mega/PAGE_SIZE) panic("Wrong allocation:");
	cprintf("Test %d Passed \n", ++count);

	freeFrames = sys_calculate_free_frames() ;
	freeDiskFrames = pf_calculate_free_frames();
	tempAddress = kmalloc(6*Mega); 			   //Use Hole 5 -> Hole 5 = 2 M
	if((uint32)tempAddress != ACTUAL_START + 0x08C00000)
		panic("Worst Fit not working correctly");
	if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
	if ((freeFrames - sys_calculate_free_frames()) !=  6*Mega/PAGE_SIZE) panic("Wrong allocation:");
	cprintf("Test %d Passed \n", ++count);

	freeFrames = sys_calculate_free_frames() ;
	freeDiskFrames = pf_calculate_free_frames();
	tempAddress = kmalloc(5*Mega); 			   //Use Hole 3 -> Hole 3 = 1 M
	if((uint32)tempAddress != ACTUAL_START + 0x01400000)
		panic("Worst Fit not working correctly");
	if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
	if ((freeFrames - sys_calculate_free_frames()) !=  5*Mega/PAGE_SIZE) panic("Wrong allocation:");
	cprintf("Test %d Passed \n", ++count);

	freeFrames = sys_calculate_free_frames() ;
	freeDiskFrames = pf_calculate_free_frames();
	tempAddress = kmalloc(4*Mega); 			   // Use Hole 4 -> Hole 4 = 1 M
	if((uint32)tempAddress != ACTUAL_START + 0x04100000)
		panic("Worst Fit not working correctly");
	if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
	if ((freeFrames - sys_calculate_free_frames()) !=  4*Mega/PAGE_SIZE) panic("Wrong allocation:");
	cprintf("Test %d Passed \n", ++count);

	freeFrames = sys_calculate_free_frames() ;
	freeDiskFrames = pf_calculate_free_frames();
	tempAddress = kmalloc(2 * Mega); 			// Use Hole 2 -> Hole 2 = 2 M
	if((uint32)tempAddress != ACTUAL_START + 0x00400000)
		panic("Worst Fit not working correctly");
	if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
	if ((freeFrames - sys_calculate_free_frames()) !=  2*Mega/PAGE_SIZE) panic("Wrong allocation:");
	cprintf("Test %d Passed \n", ++count);

	freeFrames = sys_calculate_free_frames() ;
	freeDiskFrames = pf_calculate_free_frames();
	tempAddress = kmalloc(1*Mega + 512*kilo);    // Use Hole 1 -> Hole 1 = 0.5 M
	if((uint32)tempAddress != ACTUAL_START)
		panic("Worst Fit not working correctly");
	if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
	if ((freeFrames - sys_calculate_free_frames()) !=  (1*Mega + 512*kilo)/PAGE_SIZE) panic("Wrong allocation:");
	cprintf("Test %d Passed \n", ++count);

	freeFrames = sys_calculate_free_frames() ;
	freeDiskFrames = pf_calculate_free_frames();
	tempAddress = kmalloc(512*kilo); 			   // Use Hole 2 -> Hole 2 = 1.5 M
	if((uint32)tempAddress != ACTUAL_START + 0x00600000)
		panic("Worst Fit not working correctly");
	if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
	if ((freeFrames - sys_calculate_free_frames()) !=  (512*kilo)/PAGE_SIZE) panic("Wrong allocation:");
	cprintf("Test %d Passed \n", ++count);

	freeFrames = sys_calculate_free_frames() ;
	freeDiskFrames = pf_calculate_free_frames();
	tempAddress = kmalloc(kilo); 			   // Use Hole 5 -> Hole 5 = 2 M - K
	if((uint32)tempAddress != ACTUAL_START + 0x09200000)
		panic("Worst Fit not working correctly");
	if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
	if ((freeFrames - sys_calculate_free_frames()) !=  (4*kilo)/PAGE_SIZE) panic("Wrong allocation:");
	cprintf("Test %d Passed \n", ++count);

	freeFrames = sys_calculate_free_frames() ;
	freeDiskFrames = pf_calculate_free_frames();
	tempAddress = kmalloc(2*Mega - 4*kilo); 		// Use Hole 5 -> Hole 5 = 0
	if((uint32)tempAddress != ACTUAL_START + 0x09201000)
		panic("Worst Fit not working correctly");
	if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
	if ((freeFrames - sys_calculate_free_frames()) !=  (2*Mega - 4*kilo)/PAGE_SIZE) panic("Wrong allocation:");
	cprintf("Test %d Passed \n", ++count);

	// Check that worst fit returns null in case all holes are not free
	freeFrames = sys_calculate_free_frames() ;
	freeDiskFrames = pf_calculate_free_frames();
	tempAddress = kmalloc(4*Mega); 		//No Suitable hole
	if((uint32)tempAddress != 0x0)
		panic("Worst Fit not working correctly");
	if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
	if ((freeFrames - sys_calculate_free_frames()) !=  0) panic("Wrong allocation:");
	cprintf("Test %d Passed \n", ++count);

	cprintf("Congratulations!! test Worst Fit completed successfully.\n");


	return 1;
}

int test_kfree()
{
	cprintf("==============================================\n");
	cprintf("MAKE SURE to have a FRESH RUN for this test\n(i.e. don't run any program/test before it)\n");
	cprintf("==============================================\n");

	char minByte = 1<<7;
	char maxByte = 0x7F;
	short minShort = 1<<15 ;
	short maxShort = 0x7FFF;
	int minInt = 1<<31 ;
	int maxInt = 0x7FFFFFFF;

	char *byteArr, *byteArr2 ;
	short *shortArr, *shortArr2 ;
	int *intArr;
	struct MyStruct *structArr ;
	int lastIndexOfByte, lastIndexOfByte2, lastIndexOfShort, lastIndexOfShort2, lastIndexOfInt, lastIndexOfStruct;
	int start_freeFrames = sys_calculate_free_frames() ;

	//malloc some spaces
	int i, freeFrames, freeDiskFrames ;
	char* ptr;
	int lastIndices[20] = {0};
	int sums[20] = {0};
	void* ptr_allocations[20] = {0};
	{
		//2 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[0] = kmalloc(2*Mega-kilo);
		if ((uint32) ptr_allocations[0] !=  (ACTUAL_START)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 512) panic("Wrong allocation: pages are not loaded successfully into memory");
		lastIndices[0] = (2*Mega-kilo)/sizeof(char) - 1;

		//2 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[1] = kmalloc(2*Mega-kilo);
		if ((uint32) ptr_allocations[1] != (ACTUAL_START + 2*Mega)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 512) panic("Wrong allocation: pages are not loaded successfully into memory");
		lastIndices[1] = (2*Mega-kilo)/sizeof(char) - 1;

		//2 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[2] = kmalloc(2*kilo);
		if ((uint32) ptr_allocations[2] != (ACTUAL_START + 4*Mega)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 1) panic("Wrong allocation: pages are not loaded successfully into memory");
		lastIndices[2] = (2*kilo)/sizeof(char) - 1;
		ptr = (char*)ptr_allocations[2];
		for (i = 0; i < lastIndices[2]; ++i)
		{
			ptr[i] = 2 ;
		}

		//2 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[3] = kmalloc(2*kilo);
		if ((uint32) ptr_allocations[3] != (ACTUAL_START + 4*Mega + 4*kilo)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 1) panic("Wrong allocation: pages are not loaded successfully into memory");
		lastIndices[3] = (2*kilo)/sizeof(char) - 1;
		ptr = (char*)ptr_allocations[3];
		for (i = 0; i < lastIndices[3]; ++i)
		{
			ptr[i] = 3 ;
		}

		//7 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[4] = kmalloc(7*kilo);
		if ((uint32) ptr_allocations[4] != (ACTUAL_START + 4*Mega + 8*kilo)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 2) panic("Wrong allocation: pages are not loaded successfully into memory");
		lastIndices[4] = (7*kilo)/sizeof(char) - 1;
		ptr = (char*)ptr_allocations[4];
		for (i = 0; i < lastIndices[4]; ++i)
		{
			ptr[i] = 4 ;
		}

		//3 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[5] = kmalloc(3*Mega-kilo);
		if ((uint32) ptr_allocations[5] != (ACTUAL_START + 4*Mega + 16*kilo) ) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 768) panic("Wrong allocation: pages are not loaded successfully into memory");
		lastIndices[5] = (3*Mega-kilo)/sizeof(char) - 1;
		ptr = (char*)ptr_allocations[5];
		for (i = 0; i < lastIndices[5]; ++i)
		{
			ptr[i] = 5 ;
		}

		//6 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[6] = kmalloc(6*Mega-kilo);
		if ((uint32) ptr_allocations[6] != (ACTUAL_START + 7*Mega + 16*kilo)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 1536) panic("Wrong allocation: pages are not loaded successfully into memory");
		lastIndices[6] = (6*Mega-kilo)/sizeof(char) - 1;

		//14 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[7] = kmalloc(14*kilo);
		if ((uint32) ptr_allocations[7] != (ACTUAL_START + 13*Mega + 16*kilo)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 4) panic("Wrong allocation: pages are not loaded successfully into memory");
		lastIndices[7] = (14*kilo)/sizeof(char) - 1;
		ptr = (char*)ptr_allocations[7];
		for (i = 0; i < lastIndices[7]; ++i)
		{
			ptr[i] = 7 ;
		}
	}

	//kfree some of the allocated spaces [15%]
	{
		//kfree 1st 2 MB
		int freeFrames = sys_calculate_free_frames() ;
		int freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[0]);
		if ((freeDiskFrames - pf_calculate_free_frames()) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 512 ) panic("Wrong kfree: pages in memory are not freed correctly");

		//kfree 2nd 2 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[1]);
		if ((freeDiskFrames - pf_calculate_free_frames()) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 512) panic("Wrong kfree: pages in memory are not freed correctly");

		//kfree 6 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[6]);
		if ((freeDiskFrames - pf_calculate_free_frames()) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 6*Mega/4096) panic("Wrong kfree: pages in memory are not freed correctly");
	}

	cprintf("\nkfree: current evaluation = 15%");

	//Check memory access after kfree [15%]
	{
		//2 KB
		ptr = (char*)ptr_allocations[2];
		for (i = 0; i < lastIndices[2]; ++i)
		{
			sums[2] += ptr[i] ;
		}
		if (sums[2] != 2*lastIndices[2])	panic("kfree: invalid read after freeing some allocations");

		//2 KB
		ptr = (char*)ptr_allocations[3];
		for (i = 0; i < lastIndices[3]; ++i)
		{
			sums[3] += ptr[i] ;
		}
		if (sums[3] != 3*lastIndices[3])	panic("kfree: invalid read after freeing some allocations");

		//7 KB
		ptr = (char*)ptr_allocations[4];
		for (i = 0; i < lastIndices[4]; ++i)
		{
			sums[4] += ptr[i] ;
		}
		if (sums[4] != 4*lastIndices[4])	panic("kfree: invalid read after freeing some allocations");

		//3 MB
		ptr = (char*)ptr_allocations[5];
		for (i = 0; i < lastIndices[5]; ++i)
		{
			sums[5] += ptr[i] ;
		}
		if (sums[5] != 5*lastIndices[5])	panic("kfree: invalid read after freeing some allocations");

		//14 KB
		ptr = (char*)ptr_allocations[7];
		for (i = 0; i < lastIndices[7]; ++i)
		{
			sums[7] += ptr[i] ;
		}
		if (sums[7] != 7*lastIndices[7])	panic("kfree: invalid read after freeing some allocations");
	}
	cprintf("\b\b\b30%");

	//Allocate after kfree [15%]
	{
		//20 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[8] = kmalloc(20*kilo);
		if ((uint32) ptr_allocations[8] != (ACTUAL_START + 13*Mega + 32*kilo)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 5) panic("Wrong allocation: pages are not loaded successfully into memory");
		lastIndices[8] = (20*kilo)/sizeof(char) - 1;
		ptr = (char*)ptr_allocations[8];
		for (i = 0; i < lastIndices[8]; ++i)
		{
			ptr[i] = 8 ;
		}

		//1 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[9] = kmalloc(1*Mega);
		if ((uint32) ptr_allocations[9] != (ACTUAL_START + 13*Mega + 52*kilo)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 256) panic("Wrong allocation: pages are not loaded successfully into memory");
		lastIndices[9] = (1*Mega)/sizeof(char) - 1;
		ptr = (char*)ptr_allocations[9];
		for (i = 0; i < lastIndices[9]; ++i)
		{
			ptr[i] = 9 ;
		}
	}
	cprintf("\b\b\b45%");

	//kfree remaining allocated spaces [15%]
	{
		//kfree 7 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[4]);
		if ((freeDiskFrames - pf_calculate_free_frames()) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 2) panic("Wrong kfree: pages in memory are not freed correctly");

		//kfree 3 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[5]);
		if ((freeDiskFrames - pf_calculate_free_frames()) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 3*Mega/4096) panic("Wrong kfree: pages in memory are not freed correctly");

		//kfree 1st 2 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[2]);
		if ((freeDiskFrames - pf_calculate_free_frames()) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 1) panic("Wrong kfree: pages in memory are not freed correctly");

		//kfree 2nd 2 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[3]);
		if ((freeDiskFrames - pf_calculate_free_frames()) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 1) panic("Wrong kfree: pages in memory are not freed correctly");

		//kfree 14 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[7]);
		if ((freeDiskFrames - pf_calculate_free_frames()) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 4) panic("Wrong kfree: pages in memory are not freed correctly");

		//kfree 20 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[8]);
		if ((freeDiskFrames - pf_calculate_free_frames()) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 5) panic("Wrong kfree: pages in memory are not freed correctly");

		//kfree 1 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[9]);
		if ((freeDiskFrames - pf_calculate_free_frames()) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 256) panic("Wrong kfree: pages in memory are not freed correctly");

		if(start_freeFrames != (sys_calculate_free_frames())) {panic("Wrong kfree: not all pages removed correctly at end");}
	}
	cprintf("\b\b\b60%");

	//Check memory access after kfree [15%]
	{
		//Bypass the PAGE FAULT on <MOVB immediate, reg> instruction by setting its length
		//and continue executing the remaining code
		sys_bypassPageFault(3);

		for (i = 0; i < 10; ++i)
		{
			ptr = (char *) ptr_allocations[i];
			ptr[0] = 10;
			//cprintf("\n\ncr2 = %x, faulted addr = %x", sys_rcr2(), (uint32)&(ptr[0]));
			if (sys_rcr2() != (uint32)&(ptr[0])) panic("kfree: successful access to freed space!! it should not be succeeded");
			ptr[lastIndices[i]] = 10;
			if (sys_rcr2() != (uint32)&(ptr[lastIndices[i]])) panic("kfree: successful access to freed space!! it should not be succeeded");
		}

		//set it to 0 again to cancel the bypassing option
		sys_bypassPageFault(0);
	}
	cprintf("\b\b\b75%");

	//kfree non-exist item [10%]
	{
		//kfree 2 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[0]);
		if ((freeDiskFrames - pf_calculate_free_frames()) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 0) panic("Wrong kfree: attempt to kfree a non-existing ptr. It should do nothing");

		//kfree 2 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[2]);
		if ((freeDiskFrames - pf_calculate_free_frames()) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 0) panic("Wrong kfree: attempt to kfree a non-existing ptr. It should do nothing");

		//kfree 20 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[8]);
		if ((freeDiskFrames - pf_calculate_free_frames()) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 0) panic("Wrong kfree: attempt to kfree a non-existing ptr. It should do nothing");

		//kfree 1 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[9]);
		if ((freeDiskFrames - pf_calculate_free_frames()) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 0) panic("Wrong kfree: attempt to kfree a non-existing ptr. It should do nothing");

	}
	cprintf("\b\b\b85%");

	//check tables	[15%]
	{
		long long va;
		for (va = KERNEL_HEAP_START; va < (long long)KERNEL_HEAP_MAX; va+=PTSIZE)
		{
			uint32 *ptr_table ;
			get_page_table(ptr_page_directory, (void*)(uint32)va, &ptr_table);
			if (ptr_table == NULL)
			{
				panic("Wrong kfree: one of the kernel tables is wrongly removed! Tables should not be removed here in kfree");
			}
		}
	}
	cprintf("\b\b\b100%\n");



	cprintf("\nCongratulations!! test kfree completed successfully.\n");

	return 1;

}

int test_kfree_bestfit()
{
	cprintf("==============================================\n");
	cprintf("MAKE SURE to have a FRESH RUN for this test\n(i.e. don't run any program/test before it)\n");
	cprintf("==============================================\n");

	char minByte = 1<<7;
	char maxByte = 0x7F;
	short minShort = 1<<15 ;
	short maxShort = 0x7FFF;
	int minInt = 1<<31 ;
	int maxInt = 0x7FFFFFFF;

	char *byteArr, *byteArr2 ;
	short *shortArr, *shortArr2 ;
	int *intArr;
	struct MyStruct *structArr ;
	int lastIndexOfByte, lastIndexOfByte2, lastIndexOfShort, lastIndexOfShort2, lastIndexOfInt, lastIndexOfStruct;
	int start_freeFrames = sys_calculate_free_frames() ;

	//malloc some spaces
	int i, freeFrames, freeDiskFrames ;
	char* ptr;
	int lastIndices[20] = {0};
	int sums[20] = {0};
	void* ptr_allocations[20] = {0};
	{
		//2 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[0] = kmalloc(2*Mega-kilo);
		if ((uint32) ptr_allocations[0] !=  (ACTUAL_START)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 512) panic("Wrong allocation: pages are not loaded successfully into memory");
		lastIndices[0] = (2*Mega-kilo)/sizeof(char) - 1;

		//2 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[1] = kmalloc(2*Mega-kilo);
		if ((uint32) ptr_allocations[1] != (ACTUAL_START + 2*Mega)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 512) panic("Wrong allocation: pages are not loaded successfully into memory");
		lastIndices[1] = (2*Mega-kilo)/sizeof(char) - 1;

		//2 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[2] = kmalloc(2*kilo);
		if ((uint32) ptr_allocations[2] != (ACTUAL_START + 4*Mega)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 1) panic("Wrong allocation: pages are not loaded successfully into memory");
		lastIndices[2] = (2*kilo)/sizeof(char) - 1;
		ptr = (char*)ptr_allocations[2];
		for (i = 0; i < lastIndices[2]; ++i)
		{
			ptr[i] = 2 ;
		}

		//2 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[3] = kmalloc(2*kilo);
		if ((uint32) ptr_allocations[3] != (ACTUAL_START + 4*Mega + 4*kilo)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 1) panic("Wrong allocation: pages are not loaded successfully into memory");
		lastIndices[3] = (2*kilo)/sizeof(char) - 1;
		ptr = (char*)ptr_allocations[3];
		for (i = 0; i < lastIndices[3]; ++i)
		{
			ptr[i] = 3 ;
		}

		//7 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[4] = kmalloc(7*kilo);
		if ((uint32) ptr_allocations[4] != (ACTUAL_START + 4*Mega + 8*kilo)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 2) panic("Wrong allocation: pages are not loaded successfully into memory");
		lastIndices[4] = (7*kilo)/sizeof(char) - 1;
		ptr = (char*)ptr_allocations[4];
		for (i = 0; i < lastIndices[4]; ++i)
		{
			ptr[i] = 4 ;
		}

		//3 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[5] = kmalloc(3*Mega-kilo);
		if ((uint32) ptr_allocations[5] != (ACTUAL_START + 4*Mega + 16*kilo) ) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 768) panic("Wrong allocation: pages are not loaded successfully into memory");
		lastIndices[5] = (3*Mega-kilo)/sizeof(char) - 1;
		ptr = (char*)ptr_allocations[5];
		for (i = 0; i < lastIndices[5]; ++i)
		{
			ptr[i] = 5 ;
		}

		//6 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[6] = kmalloc(6*Mega-kilo);
		if ((uint32) ptr_allocations[6] != (ACTUAL_START + 7*Mega + 16*kilo)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 1536) panic("Wrong allocation: pages are not loaded successfully into memory");
		lastIndices[6] = (6*Mega-kilo)/sizeof(char) - 1;

		//14 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[7] = kmalloc(14*kilo);
		if ((uint32) ptr_allocations[7] != (ACTUAL_START + 13*Mega + 16*kilo)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 4) panic("Wrong allocation: pages are not loaded successfully into memory");
		lastIndices[7] = (14*kilo)/sizeof(char) - 1;
		ptr = (char*)ptr_allocations[7];
		for (i = 0; i < lastIndices[7]; ++i)
		{
			ptr[i] = 7 ;
		}
	}

	//kfree some of the allocated spaces [15%]
	{
		//kfree 1st 2 MB
		int freeFrames = sys_calculate_free_frames() ;
		int freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[0]);
		if ((freeDiskFrames - pf_calculate_free_frames()) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 512 ) panic("Wrong kfree: pages in memory are not freed correctly");

		//kfree 2nd 2 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[1]);
		if ((freeDiskFrames - pf_calculate_free_frames()) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 512) panic("Wrong kfree: pages in memory are not freed correctly");

		//kfree 6 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[6]);
		if ((freeDiskFrames - pf_calculate_free_frames()) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 6*Mega/4096) panic("Wrong kfree: pages in memory are not freed correctly");
	}

	cprintf("\nkfree: current evaluation = 15%");

	//Check memory access after kfree [15%]
	{
		//2 KB
		ptr = (char*)ptr_allocations[2];
		for (i = 0; i < lastIndices[2]; ++i)
		{
			sums[2] += ptr[i] ;
		}
		if (sums[2] != 2*lastIndices[2])	panic("kfree: invalid read after freeing some allocations");

		//2 KB
		ptr = (char*)ptr_allocations[3];
		for (i = 0; i < lastIndices[3]; ++i)
		{
			sums[3] += ptr[i] ;
		}
		if (sums[3] != 3*lastIndices[3])	panic("kfree: invalid read after freeing some allocations");

		//7 KB
		ptr = (char*)ptr_allocations[4];
		for (i = 0; i < lastIndices[4]; ++i)
		{
			sums[4] += ptr[i] ;
		}
		if (sums[4] != 4*lastIndices[4])	panic("kfree: invalid read after freeing some allocations");

		//3 MB
		ptr = (char*)ptr_allocations[5];
		for (i = 0; i < lastIndices[5]; ++i)
		{
			sums[5] += ptr[i] ;
		}
		if (sums[5] != 5*lastIndices[5])	panic("kfree: invalid read after freeing some allocations");

		//14 KB
		ptr = (char*)ptr_allocations[7];
		for (i = 0; i < lastIndices[7]; ++i)
		{
			sums[7] += ptr[i] ;
		}
		if (sums[7] != 7*lastIndices[7])	panic("kfree: invalid read after freeing some allocations");
	}
	cprintf("\b\b\b30%");

	//Allocate after kfree [15%]
	{
		//20 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[8] = kmalloc(20*kilo);
		if ((uint32) ptr_allocations[8] != (ACTUAL_START)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 5) panic("Wrong allocation: pages are not loaded successfully into memory");
		lastIndices[8] = (20*kilo)/sizeof(char) - 1;
		ptr = (char*)ptr_allocations[8];
		for (i = 0; i < lastIndices[8]; ++i)
		{
			ptr[i] = 8 ;
		}

		//1 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[9] = kmalloc(1*Mega);
		if ((uint32) ptr_allocations[9] != (ACTUAL_START + 20*kilo)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 256) panic("Wrong allocation: pages are not loaded successfully into memory");
		lastIndices[9] = (1*Mega)/sizeof(char) - 1;
		ptr = (char*)ptr_allocations[9];
		for (i = 0; i < lastIndices[9]; ++i)
		{
			ptr[i] = 9 ;
		}
	}
	cprintf("\b\b\b45%");

	//kfree remaining allocated spaces [15%]
	{
		//kfree 7 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[4]);
		if ((freeDiskFrames - pf_calculate_free_frames()) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 2) panic("Wrong kfree: pages in memory are not freed correctly");

		//kfree 3 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[5]);
		if ((freeDiskFrames - pf_calculate_free_frames()) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 3*Mega/4096) panic("Wrong kfree: pages in memory are not freed correctly");

		//kfree 1st 2 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[2]);
		if ((freeDiskFrames - pf_calculate_free_frames()) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 1) panic("Wrong kfree: pages in memory are not freed correctly");

		//kfree 2nd 2 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[3]);
		if ((freeDiskFrames - pf_calculate_free_frames()) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 1) panic("Wrong kfree: pages in memory are not freed correctly");

		//kfree 14 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[7]);
		if ((freeDiskFrames - pf_calculate_free_frames()) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 4) panic("Wrong kfree: pages in memory are not freed correctly");

		//kfree 20 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[8]);
		if ((freeDiskFrames - pf_calculate_free_frames()) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 5) panic("Wrong kfree: pages in memory are not freed correctly");

		//kfree 1 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[9]);
		if ((freeDiskFrames - pf_calculate_free_frames()) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 256) panic("Wrong kfree: pages in memory are not freed correctly");

		if(start_freeFrames != (sys_calculate_free_frames())) {panic("Wrong kfree: not all pages removed correctly at end");}
	}
	cprintf("\b\b\b60%");

	//Check memory access after kfree [15%]
	{
		//Bypass the PAGE FAULT on <MOVB immediate, reg> instruction by setting its length
		//and continue executing the remaining code
		sys_bypassPageFault(3);

		for (i = 0; i < 10; ++i)
		{
			ptr = (char *) ptr_allocations[i];
			ptr[0] = 10;
			//cprintf("\n\ncr2 = %x, faulted addr = %x", sys_rcr2(), (uint32)&(ptr[0]));
			if (sys_rcr2() != (uint32)&(ptr[0])) panic("kfree: successful access to freed space!! it should not be succeeded");
			ptr[lastIndices[i]] = 10;
			if (sys_rcr2() != (uint32)&(ptr[lastIndices[i]])) panic("kfree: successful access to freed space!! it should not be succeeded");
		}

		//set it to 0 again to cancel the bypassing option
		sys_bypassPageFault(0);
	}
	cprintf("\b\b\b75%");

	//kfree non-exist item [10%]
	{
		//kfree 2 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[0]);
		if ((freeDiskFrames - pf_calculate_free_frames()) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 0) panic("Wrong kfree: attempt to kfree a non-existing ptr. It should do nothing");

		//kfree 2 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[2]);
		if ((freeDiskFrames - pf_calculate_free_frames()) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 0) panic("Wrong kfree: attempt to kfree a non-existing ptr. It should do nothing");

		//kfree 20 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[8]);
		if ((freeDiskFrames - pf_calculate_free_frames()) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 0) panic("Wrong kfree: attempt to kfree a non-existing ptr. It should do nothing");

		//kfree 1 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[9]);
		if ((freeDiskFrames - pf_calculate_free_frames()) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 0) panic("Wrong kfree: attempt to kfree a non-existing ptr. It should do nothing");

	}
	cprintf("\b\b\b85%");

	//check tables	[15%]
	{
		long long va;
		for (va = KERNEL_HEAP_START; va < (long long)KERNEL_HEAP_MAX; va+=PTSIZE)
		{
			uint32 *ptr_table ;
			get_page_table(ptr_page_directory, (void*)(uint32)va, &ptr_table);
			if (ptr_table == NULL)
			{
				panic("Wrong kfree: one of the kernel tables is wrongly removed! Tables should not be removed here in kfree");
			}
		}
	}
	cprintf("\b\b\b100%\n");



	cprintf("\nCongratulations!! test kfree completed successfully.\n");

	return 1;

}

int test_kheap_phys_addr()
{
	cprintf("==============================================\n");
	cprintf("MAKE SURE to have a FRESH RUN for this test\n(i.e. don't run any program/test before it)\n");
	cprintf("==============================================\n");

	char minByte = 1<<7;
	char maxByte = 0x7F;
	short minShort = 1<<15 ;
	short maxShort = 0x7FFF;
	int minInt = 1<<31 ;
	int maxInt = 0x7FFFFFFF;

	char *byteArr, *byteArr2 ;
	short *shortArr, *shortArr2 ;
	int *intArr;
	struct MyStruct *structArr ;
	int lastIndexOfByte, lastIndexOfByte2, lastIndexOfShort, lastIndexOfShort2, lastIndexOfInt, lastIndexOfStruct;
	int start_freeFrames = sys_calculate_free_frames() ;

	//malloc some spaces
	int i, freeFrames, freeDiskFrames ;
	char* ptr;
	int lastIndices[20] = {0};
	int sums[20] = {0};
	void* ptr_allocations[20] = {0};
	{
		//2 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[0] = kmalloc(2*Mega-kilo);
		if ((uint32) ptr_allocations[0] !=  (ACTUAL_START)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 512) panic("Wrong allocation: pages are not loaded successfully into memory");

		//2 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[1] = kmalloc(2*Mega-kilo);
		if ((uint32) ptr_allocations[1] != (ACTUAL_START + 2*Mega)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 512) panic("Wrong allocation: pages are not loaded successfully into memory");

		//2 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[2] = kmalloc(2*kilo);
		if ((uint32) ptr_allocations[2] != (ACTUAL_START + 4*Mega)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 1) panic("Wrong allocation: pages are not loaded successfully into memory");

		//2 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[3] = kmalloc(2*kilo);
		if ((uint32) ptr_allocations[3] != (ACTUAL_START + 4*Mega + 4*kilo)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 1) panic("Wrong allocation: pages are not loaded successfully into memory");

		//7 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[4] = kmalloc(7*kilo);
		if ((uint32) ptr_allocations[4] != (ACTUAL_START + 4*Mega + 8*kilo)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 2) panic("Wrong allocation: pages are not loaded successfully into memory");

		//3 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[5] = kmalloc(3*Mega-kilo);
		if ((uint32) ptr_allocations[5] != (ACTUAL_START + 4*Mega + 16*kilo) ) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 768) panic("Wrong allocation: pages are not loaded successfully into memory");

		//6 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[6] = kmalloc(6*Mega-kilo);
		if ((uint32) ptr_allocations[6] != (ACTUAL_START + 7*Mega + 16*kilo)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 1536) panic("Wrong allocation: pages are not loaded successfully into memory");

		//14 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[7] = kmalloc(14*kilo);
		if ((uint32) ptr_allocations[7] != (ACTUAL_START + 13*Mega + 16*kilo)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 4) panic("Wrong allocation: pages are not loaded successfully into memory");
	}

	//test kheap_physical_address after kmalloc only [40%]
	{
		uint32 va;
		uint32 endVA = ACTUAL_START + 13*Mega + 32*kilo;
		uint32 allPAs[(13*Mega + 32*kilo+INITIAL_KHEAB_ALLOCATIONS)/PAGE_SIZE] ;
		i = 0;
		for (va = KERNEL_HEAP_START; va < endVA; va+=PAGE_SIZE)
		{
			allPAs[i++] = kheap_physical_address(va);
		}
		int ii = i ;
		i = 0;
		int j;
		for (va = KERNEL_HEAP_START; va < endVA; va+=PTSIZE)
		{
			uint32 *ptr_table ;
			get_page_table(ptr_page_directory, (void*)va, &ptr_table);
			if (ptr_table == NULL)
				panic("one of the kernel tables is wrongly removed! Tables of Kernel Heap should not be removed");

			for (j = 0; i < ii && j < 1024; ++j, ++i)
			{
				if ((ptr_table[j] & 0xFFFFF000) != allPAs[i])
				{
					//cprintf("\nVA = %x, table entry = %x, khep_pa = %x\n",va + j*PAGE_SIZE, (ptr_table[j] & 0xFFFFF000) , allPAs[i]);
					panic("Wrong kheap_physical_address");
				}
			}
		}
	}
	cprintf("\nkheap_physical_address: current evaluation = 40%");

	//kfree some of the allocated spaces
	{
		//kfree 1st 2 MB
		int freeFrames = sys_calculate_free_frames() ;
		int freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[0]);
		if ((freeDiskFrames - pf_calculate_free_frames()) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 512 ) panic("Wrong kfree: pages in memory are not freed correctly");

		//kfree 2nd 2 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[1]);
		if ((freeDiskFrames - pf_calculate_free_frames()) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 512) panic("Wrong kfree: pages in memory are not freed correctly");

		//kfree 6 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[6]);
		if ((freeDiskFrames - pf_calculate_free_frames()) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 6*Mega/4096) panic("Wrong kfree: pages in memory are not freed correctly");
	}


	//test kheap_physical_address after kmalloc and kfree [40%]
	{
		uint32 va;
		uint32 endVA = ACTUAL_START + 13*Mega + 32*kilo;
		uint32 allPAs[(13*Mega + 32*kilo+INITIAL_KHEAB_ALLOCATIONS)/PAGE_SIZE] ;
		i = 0;
		for (va = KERNEL_HEAP_START; va < endVA; va+=PAGE_SIZE)
		{
			allPAs[i++] = kheap_physical_address(va);
		}
		int ii = i ;
		i = 0;
		int j;
		for (va = KERNEL_HEAP_START; va < endVA; va+=PTSIZE)
		{
			uint32 *ptr_table ;
			get_page_table(ptr_page_directory, (void*)va, &ptr_table);
			if (ptr_table == NULL)
				panic("one of the kernel tables is wrongly removed! Tables of Kernel Heap should not be removed");

			for (j = 0; i < ii && j < 1024; ++j, ++i)
			{
				if ((ptr_table[j] & 0xFFFFF000) != allPAs[i])
				{
					//cprintf("\nVA = %x, table entry = %x, khep_pa = %x\n",va + j*PAGE_SIZE, (ptr_table[j] & 0xFFFFF000) , allPAs[i]);
					panic("Wrong kheap_physical_address");
				}
			}
		}
	}

	cprintf("\b\b\b80%");

	//test kheap_physical_address on non-mapped area [20%]
	{
		uint32 va;
		uint32 startVA = ACTUAL_START + 16*Mega;
		i = 0;
		for (va = startVA; va < KERNEL_HEAP_MAX; va+=PAGE_SIZE)
		{
			i++;
		}
		int ii = i ;
		i = 0;
		int j;
		long long va2;
		for (va2 = startVA; va2 < (long long)KERNEL_HEAP_MAX; va2+=PTSIZE)
		{
			uint32 *ptr_table ;
			get_page_table(ptr_page_directory, (void*)(uint32)va2, &ptr_table);
			if (ptr_table == NULL)
			{
				panic("one of the kernel tables is wrongly removed! Tables of Kernel Heap should not be removed");
			}
			for (j = 0; i < ii && j < 1024; ++j, ++i)
			{
				//if ((ptr_table[j] & 0xFFFFF000) != allPAs[i])
				unsigned int page_va = startVA+i*PAGE_SIZE;
				unsigned int supposed_kheap_phys_add = kheap_physical_address(page_va);
				if ((ptr_table[j] & 0xFFFFF000) != supposed_kheap_phys_add)
				{
					//cprintf("\nVA = %x, table entry = %x, khep_pa = %x\n",va2 + j*PAGE_SIZE, (ptr_table[j] & 0xFFFFF000) , allPAs[i]);
					panic("Wrong kheap_physical_address");
				}
			}
		}
	}

	cprintf("\b\b\b100%\n");

	cprintf("\nCongratulations!! test kheap_physical_address completed successfully.\n");

	return 1;

}

int test_kheap_virt_addr()
{
	cprintf("==============================================\n");
	cprintf("MAKE SURE to have a FRESH RUN for this test\n(i.e. don't run any program/test before it)\n");
	cprintf("==============================================\n");

	char minByte = 1<<7;
	char maxByte = 0x7F;
	short minShort = 1<<15 ;
	short maxShort = 0x7FFF;
	int minInt = 1<<31 ;
	int maxInt = 0x7FFFFFFF;

	char *byteArr, *byteArr2 ;
	short *shortArr, *shortArr2 ;
	int *intArr;
	struct MyStruct *structArr ;
	int lastIndexOfByte, lastIndexOfByte2, lastIndexOfShort, lastIndexOfShort2, lastIndexOfInt, lastIndexOfStruct;
	int start_freeFrames = sys_calculate_free_frames() ;

	//malloc some spaces
	int i, freeFrames, freeDiskFrames ;
	char* ptr;
	int lastIndices[20] = {0};
	int sums[20] = {0};
	void* ptr_allocations[20] = {0};
	{
		//2 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[0] = kmalloc(2*Mega-kilo);
		if ((uint32) ptr_allocations[0] !=  (ACTUAL_START)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 512) panic("Wrong allocation: pages are not loaded successfully into memory");

		//2 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[1] = kmalloc(2*Mega-kilo);
		if ((uint32) ptr_allocations[1] != (ACTUAL_START + 2*Mega)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 512) panic("Wrong allocation: pages are not loaded successfully into memory");

		//2 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[2] = kmalloc(2*kilo);
		if ((uint32) ptr_allocations[2] != (ACTUAL_START + 4*Mega)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 1) panic("Wrong allocation: pages are not loaded successfully into memory");

		//2 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[3] = kmalloc(2*kilo);
		if ((uint32) ptr_allocations[3] != (ACTUAL_START + 4*Mega + 4*kilo)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 1) panic("Wrong allocation: pages are not loaded successfully into memory");

		//7 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[4] = kmalloc(7*kilo);
		if ((uint32) ptr_allocations[4] != (ACTUAL_START + 4*Mega + 8*kilo)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 2) panic("Wrong allocation: pages are not loaded successfully into memory");

		//3 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[5] = kmalloc(3*Mega-kilo);
		if ((uint32) ptr_allocations[5] != (ACTUAL_START + 4*Mega + 16*kilo) ) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 768) panic("Wrong allocation: pages are not loaded successfully into memory");

		//6 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[6] = kmalloc(6*Mega-kilo);
		if ((uint32) ptr_allocations[6] != (ACTUAL_START + 7*Mega + 16*kilo)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 1536) panic("Wrong allocation: pages are not loaded successfully into memory");

		//14 KB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		ptr_allocations[7] = kmalloc(14*kilo);
		if ((uint32) ptr_allocations[7] != (ACTUAL_START + 13*Mega + 16*kilo)) panic("Wrong start address for the allocated space... check return address of kmalloc & updating of heap ptr");
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((freeFrames - sys_calculate_free_frames()) != 4) panic("Wrong allocation: pages are not loaded successfully into memory");
	}

	uint32 allPAs[(13*Mega + 32*kilo+ INITIAL_KHEAB_ALLOCATIONS)/PAGE_SIZE] ;
	int numOfFrames = (13*Mega + 32*kilo+INITIAL_KHEAB_ALLOCATIONS)/PAGE_SIZE ;

	//test kheap_virtual_address after kmalloc only [40%]
	{
		uint32 va;
		uint32 endVA = ACTUAL_START + 13*Mega + 32*kilo;
		int i = 0;
		int j;
		for (va = KERNEL_HEAP_START; va < endVA; va+=PTSIZE)
		{
			uint32 *ptr_table ;
			get_page_table(ptr_page_directory, (void*)va, &ptr_table);
			if (ptr_table == NULL)
				panic("one of the kernel tables is wrongly removed! Tables of Kernel Heap should not be removed");

			for (j = 0; i < numOfFrames && j < 1024; ++j, ++i)
			{
				allPAs[i] = (ptr_table[j] & 0xFFFFF000);
				uint32 retrievedVA = kheap_virtual_address(allPAs[i]);

				if (retrievedVA != (va + j*PAGE_SIZE))
				{
					//cprintf("\nVA = %x, table entry = %x, khep_pa = %x\n",va + j*PAGE_SIZE, (ptr_table[j] & 0xFFFFF000) , allPAs[i]);
					panic("Wrong kheap_virtual_address");
				}
			}
		}
	}
	cprintf("\nkheap_virtual_address: current evaluation = 40%");

	//kfree some of the allocated spaces
	{
		//kfree 1st 2 MB
		int freeFrames = sys_calculate_free_frames() ;
		int freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[0]);
		if ((freeDiskFrames - pf_calculate_free_frames()) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 512 ) panic("Wrong kfree: pages in memory are not freed correctly");

		//kfree 2nd 2 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[1]);
		if ((freeDiskFrames - pf_calculate_free_frames()) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 512) panic("Wrong kfree: pages in memory are not freed correctly");

		//kfree 6 MB
		freeFrames = sys_calculate_free_frames() ;
		freeDiskFrames = pf_calculate_free_frames() ;
		kfree(ptr_allocations[6]);
		if ((freeDiskFrames - pf_calculate_free_frames()) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		if ((sys_calculate_free_frames() - freeFrames) != 6*Mega/4096) panic("Wrong kfree: pages in memory are not freed correctly");
	}


	//test kheap_virtual_address after kmalloc and kfree [40%]
	{
		uint32 va;
		uint32 endVA = ACTUAL_START + 13*Mega + 32*kilo;
		int i = 0;
		int j;
		//frames of first 4 MB
		uint32 startIndex = (INITIAL_KHEAB_ALLOCATIONS) / PAGE_SIZE;
		for (i = startIndex ; i < startIndex + 4*Mega/PAGE_SIZE; ++i)
		{
			uint32 retrievedVA = kheap_virtual_address(allPAs[i]);
			if (retrievedVA != 0)
			{
				panic("Wrong kheap_virtual_address");
			}

		}
		//next frames until 6 MB
		for (i = startIndex + 4*Mega/PAGE_SIZE; i < startIndex + (7*Mega + 16*kilo)/PAGE_SIZE; ++i)
		{
			uint32 retrievedVA = kheap_virtual_address(allPAs[i]);
			if (retrievedVA != KERNEL_HEAP_START + i*PAGE_SIZE)
			{
				panic("Wrong kheap_virtual_address");
			}
		}
		//frames of 6 MB
		for (i = startIndex + (7*Mega + 16*kilo)/PAGE_SIZE; i < startIndex + (13*Mega + 16*kilo)/PAGE_SIZE; ++i)
		{
			uint32 retrievedVA = kheap_virtual_address(allPAs[i]);
			if (retrievedVA != 0)
			{
				panic("Wrong kheap_virtual_address");
			}
		}
		//frames of last allocation (14 KB)
		for (i = startIndex + (13*Mega + 16*kilo)/PAGE_SIZE; i < startIndex + (13*Mega + 32*kilo)/PAGE_SIZE; ++i)
		{
			uint32 retrievedVA = kheap_virtual_address(allPAs[i]);
			if (retrievedVA != KERNEL_HEAP_START + i*PAGE_SIZE)
			{
				panic("Wrong kheap_virtual_address");
			}
		}
	}

	cprintf("\b\b\b80%");

	//test kheap_virtual_address on frames of KERNEL CODE [20%]
	{
		uint32 i;
		for (i = 1*Mega; i < (uint32)(end_of_kernel - KERNEL_BASE); i+=PAGE_SIZE)
		{
			uint32 retrievedVA = kheap_virtual_address(i);
			if (retrievedVA != 0)
			{
				cprintf("\nPA = %x, retrievedVA = %x\n", i, retrievedVA);
				panic("Wrong kheap_virtual_address");
			}
		}
	}

	cprintf("\b\b\b100%\n");

	cprintf("\nCongratulations!! test kheap_virtual_address completed successfully.\n");

	return 1;

}

int initFreeFrames;
int initFreeDiskFrames ;
uint8 firstCall = 1 ;
int test_three_creation_functions()
{
	if (firstCall)
	{
		firstCall = 0;
		initFreeFrames = sys_calculate_free_frames() ;
		initFreeDiskFrames = pf_calculate_free_frames() ;
		//Run simple user program
		{
			char command[100] = "run fos_add 4096";
			execute_command(command) ;
		}
	}
	//Ensure that the user directory, page WS and page tables are allocated in KERNEL HEAP
	{
		struct Env * e = NULL;
		struct Env * ptr_env = NULL;
		LIST_FOREACH(ptr_env, &env_exit_queue)
		{
			if (strcmp(ptr_env->prog_name, "fos_add") == 0)
			{
				e = ptr_env ;
				break;
			}
		}
		if (e->pageFaultsCounter != 0)
			panic("Page fault is occur while not expected to. Review the three creation functions");

		int pagesInWS = env_page_ws_get_size(e);
		int curFreeFrames = sys_calculate_free_frames() ;
		int curFreeDiskFrames = pf_calculate_free_frames() ;
		//cprintf("\ndiff in page file = %d, pages in WS = %d\n", initFreeDiskFrames - curFreeDiskFrames, pagesInWS);
		if ((initFreeDiskFrames - curFreeDiskFrames) != pagesInWS) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
		//cprintf("\ndiff in mem frames = %d, pages in WS = %d\n", initFreeFrames - curFreeFrames, pagesInWS);
		if ((initFreeFrames - curFreeFrames) != 12/*WS*/ + 2*1/*DIR*/ + 2*3/*Tables*/ + 1 /*user WS table*/ + pagesInWS) panic("Wrong allocation: pages are not loaded successfully into memory");

		//allocate 4 KB
		char *ptr = kmalloc(4*kilo);
		if ((uint32) ptr !=  (ACTUAL_START + (12+2*1+2*3+1)*PAGE_SIZE)) panic("Wrong start address for the allocated space... make sure you create the dir, table and page WS in KERNEL HEAP");
	}

	cprintf("\nCongratulations!! test the 3 creation functions is completed successfully.\n");

	return 1;
}

