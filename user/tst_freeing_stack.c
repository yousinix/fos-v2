/*NOTE=================================================*/
//Run it two separate times:
//	First: with PAGE WS = 1000 [Placement]
//	Second: with PAGE WS = 10 [Replacement]
//In each case, it should print the successful message
/*=====================================================*/

#include <inc/lib.h>

int RecursiveFn(int numOfRec);
void _main(void)
{
	int res, numOfRec, expectedResult, r, i, j, freeFrames, usedDiskPages ;
	uint32 vaOf1stStackPage = USTACKTOP - PAGE_SIZE;

	int initNumOfEmptyWSEntries, curNumOfEmptyWSEntries ;

	/*Different number of recursive calls (each call takes 1 PAGE)*/
	for (r = 1; r <= 10; ++r)
	{
		numOfRec = r;

		initNumOfEmptyWSEntries = 0;
		for (j = 0; j < myEnv->page_WS_max_size; ++j)
		{
			if (myEnv->__uptr_pws[j].empty==1)
				initNumOfEmptyWSEntries++;
		}

		freeFrames = sys_calculate_free_frames() ;
		usedDiskPages = sys_pf_calculate_allocated_pages() ;

		res = RecursiveFn(numOfRec);
		env_sleep(1) ;
		expectedResult = 0;
		for (i = 1; i <= numOfRec; ++i) {
			expectedResult += i * 1024;
		}
		//check correct answer & page file
		if (res != expectedResult) panic("Wrong result of the recursive function!\n");
		if ((usedDiskPages - sys_pf_calculate_allocated_pages()) != 0) panic("Wrong freeing the stack pages from the page file!\n");

		//check WS
		for (i = 1; i <= numOfRec; ++i)
		{
			for (j = 0; j < myEnv->page_WS_max_size; ++j)
			{
				if (ROUNDDOWN(myEnv->__uptr_pws[j].virtual_address, PAGE_SIZE) == vaOf1stStackPage - i*PAGE_SIZE)
					panic("Wrong freeing the stack pages from the working set!\n");
			}
		}

		//check free frames
		curNumOfEmptyWSEntries = 0;
		for (j = 0; j < myEnv->page_WS_max_size; ++j)
		{
			if (myEnv->__uptr_pws[j].empty==1)
				curNumOfEmptyWSEntries++;
		}

		//cprintf("diff in RAM = %d\n", sys_calculate_free_frames() - freeFrames);
		if ((sys_calculate_free_frames() - freeFrames) != curNumOfEmptyWSEntries - initNumOfEmptyWSEntries)
			panic("Wrong freeing the stack pages from memory!\n");
	}

	cprintf("Congratulations!! test freeing the stack pages is completed successfully.\n");

	return;
}

int RecursiveFn(int numOfRec)
{
	if (numOfRec == 0)
		return 0;

	int A[1024] ;
	int i, sum = 0 ;
	for (i = 0; i < 1024; ++i) {
		A[i] = numOfRec;
	}
	for (i = 0; i < 1024; ++i) {
		sum += A[i] ;
	}
	return sum + RecursiveFn(numOfRec-1);
}
