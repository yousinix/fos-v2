// tests that modified list is cleared and updated
/* *********************************************************** */
/* MAKE SURE PAGE_WS_MAX_SIZE = 9 */
/* MAKE SURE MAX_MODIFIED_LIST_COUNT = 10*/
/* *********************************************************** */

#include <inc/lib.h>

/*SHOULD be on User DATA not on the STACK*/
char arr[PAGE_SIZE*1024*14 + PAGE_SIZE];
//=========================================

void _main(void)
{

	//	cprintf("envID = %d\n",envID);




	/*[1] CHECK INITIAL WORKING SET*/
	{
		if( ROUNDDOWN(myEnv->__uptr_pws[0].virtual_address,PAGE_SIZE) !=   0x200000)  	panic("INITIAL PAGE WS entry checking failed! Review sizes of the two WS's..!!");
		if( ROUNDDOWN(myEnv->__uptr_pws[1].virtual_address,PAGE_SIZE) !=   0x201000)  panic("INITIAL PAGE WS entry checking failed! Review sizes of the two WS's..!!");
		if( ROUNDDOWN(myEnv->__uptr_pws[2].virtual_address,PAGE_SIZE) !=   0x202000)  panic("INITIAL PAGE WS entry checking failed! Review sizes of the two WS's..!!");
		if( ROUNDDOWN(myEnv->__uptr_pws[3].virtual_address,PAGE_SIZE) !=   0x203000)  panic("INITIAL PAGE WS entry checking failed! Review sizes of the two WS's..!!");
		if( ROUNDDOWN(myEnv->__uptr_pws[4].virtual_address,PAGE_SIZE) !=   0x204000)  panic("INITIAL PAGE WS entry checking failed! Review sizes of the two WS's..!!");
		if( ROUNDDOWN(myEnv->__uptr_pws[5].virtual_address,PAGE_SIZE) !=   0x205000)  panic("INITIAL PAGE WS entry checking failed! Review sizes of the two WS's..!!");
		if( ROUNDDOWN(myEnv->__uptr_pws[6].virtual_address,PAGE_SIZE) !=   0x800000)  panic("INITIAL PAGE WS entry checking failed! Review sizes of the two WS's..!!");
		if( ROUNDDOWN(myEnv->__uptr_pws[7].virtual_address,PAGE_SIZE) !=   0x801000)  panic("INITIAL PAGE WS entry checking failed! Review sizes of the two WS's..!!");
		//if( ROUNDDOWN(myEnv->__uptr_pws[7].virtual_address,PAGE_SIZE) !=   0x802000)  panic("INITIAL PAGE WS entry checking failed! Review sizes of the two WS's..!!");
		if( ROUNDDOWN(myEnv->__uptr_pws[8].virtual_address,PAGE_SIZE) !=   0x802000)  panic("INITIAL PAGE WS entry checking failed! Review sizes of the two WS's..!!");
		if( ROUNDDOWN(myEnv->__uptr_pws[9].virtual_address,PAGE_SIZE) !=   0x803000)  panic("INITIAL PAGE WS entry checking failed! Review sizes of the two WS's..!!");
		if( ROUNDDOWN(myEnv->__uptr_pws[10].virtual_address,PAGE_SIZE) !=   0xeebfd000)  panic("INITIAL PAGE WS entry checking failed! Review sizes of the two WS's..!!");
		if( myEnv->page_last_RS_index !=  0)  										panic("INITIAL PAGE WS last index checking failed! Review sizes of the two WS's..!!");
	}

	/*[2] RUN THE SLAVE PROGRAM*/

	//****************************************************************************************************************
	//IMP: program name is placed statically on the stack to avoid PAGE FAULT on it during the sys call inside the Kernel
	char slaveProgName[10] = "tpb2slave";
	//****************************************************************************************************************

	int32 envIdSlave = sys_create_env(slaveProgName, (myEnv->page_WS_max_size), (myEnv->percentage_of_WS_pages_to_be_removed));
	int initModBufCnt = sys_calculate_modified_frames();
	sys_run_env(envIdSlave);

	/*[3] BUSY-WAIT FOR A WHILE TILL FINISHING IT*/
	env_sleep(5000);


	//NOW: modified list contains 7 pages from the slave program
	if (sys_calculate_modified_frames() - initModBufCnt  != 7)  panic("Error in BUFFERING/RESTORING of modified frames of the slave ... WRONG number of buffered pages in MODIFIED frame list");


	/*START OF TST_BUFFER_2*/
	int usedDiskPages = sys_pf_calculate_allocated_pages() ;
	int freePages = sys_calculate_free_frames();
	initModBufCnt = sys_calculate_modified_frames();
	int initFreeBufCnt = sys_calculate_notmod_frames();
	int dummy = 0;

	int i=0;
	for(;i<1;i++)
	{
		arr[i] = -1;
	}
	dummy = sys_calculate_notmod_frames() + myEnv->env_id;	//Always use page #: 800000, 801000, 803000

	//Fault #1
	i=PAGE_SIZE*1024;
	for(;i<PAGE_SIZE*1024+1;i++)
	{
		arr[i] = -1;
	}
	dummy = sys_calculate_notmod_frames() + myEnv->env_id;	//Always use page #: 800000, 801000, 803000

	//Fault #2
	i=PAGE_SIZE*1024*2;
	for(;i<PAGE_SIZE*1024*2+1;i++)
	{
		arr[i] = -1;
	}
	dummy = sys_calculate_notmod_frames() + myEnv->env_id;	//Always use page #: 800000, 801000, 803000

	//Fault #3
	i=PAGE_SIZE*1024*3;
	for(;i<PAGE_SIZE*1024*3+1;i++)
	{
		arr[i] = -1;
	}
	dummy = sys_calculate_notmod_frames() + myEnv->env_id;	//Always use page #: 800000, 801000, 803000

	//Fault #4
	i=PAGE_SIZE*1024*4;
	for(;i<PAGE_SIZE*1024*4+1;i++)
	{
		arr[i] = -1;
	}
	dummy = sys_calculate_notmod_frames() + myEnv->env_id;	//Always use page #: 800000, 801000, 803000

	//Fault #5
	i=PAGE_SIZE*1024*5;
	for(;i<PAGE_SIZE*1024*5+1;i++)
	{
		arr[i] = -1;
	}
	dummy = sys_calculate_notmod_frames() + myEnv->env_id;	//Always use page #: 800000, 801000, 803000

	//Fault #6
	i=PAGE_SIZE*1024*6;
	for(;i<PAGE_SIZE*1024*6+1;i++)
	{
		arr[i] = -1;
	}
	dummy = sys_calculate_notmod_frames() + myEnv->env_id;	//Always use page #: 800000, 801000, 803000

	//Fault #7
	i=PAGE_SIZE*1024*7;
	for(;i<PAGE_SIZE*1024*7+1;i++)
	{
		arr[i] = -1;
	}
	dummy = sys_calculate_notmod_frames() + myEnv->env_id;	//Always use page #: 800000, 801000, 803000

	//TILL NOW: 6 pages were brought into MEM and be modified (6 unmodified should be buffered)
	if (sys_calculate_notmod_frames()  - initFreeBufCnt != 7)
	{
		/*sys_env_destroy(envIdSlave);*/
		panic("Error in BUFFERING/RESTORING of free frames... WRONG number of buffered pages in FREE frame list %d",sys_calculate_notmod_frames()  - initFreeBufCnt);
	}
	if (sys_calculate_modified_frames() - initModBufCnt  != 0)
	{
		/*sys_env_destroy(envIdSlave);*/
		panic("Error in BUFFERING/RESTORING of modified frames... WRONG number of buffered pages in MODIFIED frame list");
	}

	initFreeBufCnt = sys_calculate_notmod_frames();

	//The following 7 faults should victimize the 7 previously modified pages
	//(i.e. the modified list should be freed after 3 faults... then, two modified frames will be added to it again)
	//Fault #7
	i=PAGE_SIZE*1024*8;
	int s = 0;
	for(;i<PAGE_SIZE*1024*8+1;i++)
	{
		s += arr[i] ;
	}
	dummy = sys_calculate_notmod_frames() + myEnv->env_id;	//Always use page #: 800000, 801000, 803000

	//Fault #8
	i=PAGE_SIZE*1024*9;
	for(;i<PAGE_SIZE*1024*9+1;i++)
	{
		s += arr[i] ;
	}
	dummy = sys_calculate_notmod_frames() + myEnv->env_id;	//Always use page #: 800000, 801000, 803000

	//Fault #9
	i=PAGE_SIZE*1024*10;
	for(;i<PAGE_SIZE*1024*10+1;i++)
	{
		s += arr[i] ;
	}

	//dummy = sys_calculate_notmod_frames() + myEnv->env_id;	//Always use page #: 800000, 801000, 803000

	//HERE: modified list should be freed
	if (sys_calculate_modified_frames() != 0)
	{
		/*sys_env_destroy(envIdSlave);*/
		panic("Modified frames not removed from list (or not updated) correctly when the modified list reaches MAX %d", sys_calculate_modified_frames());
	}
	if ((sys_calculate_notmod_frames() - initFreeBufCnt) != 10)
	{
		/*sys_env_destroy(envIdSlave);*/
		panic("Modified frames not added to free frame list as BUFFERED when the modified list reaches MAX");
	}

	//Three additional fault (i.e. three modified page will be added to modified list)
	//Fault #10
	i = PAGE_SIZE * 1024 * 11;
	for (; i < PAGE_SIZE * 1024*11 + 1; i++) {
		s += arr[i] ;
	}
	dummy = sys_calculate_notmod_frames() + myEnv->env_id;	//Always use page #: 800000, 801000, 803000

	//Fault #11
	i = PAGE_SIZE * 1024 * 12;
	for (; i < PAGE_SIZE * 1024*12 + 1; i++) {
		s += arr[i] ;
	}
	dummy = sys_calculate_notmod_frames() + myEnv->env_id;	//Always use page #: 800000, 801000, 803000

	//Fault #12
	i = PAGE_SIZE * 1024 * 13;
	for (; i < PAGE_SIZE * 1024*13 + 1; i++) {
		s += arr[i] ;
	}
	//dummy = sys_calculate_notmod_frames() + myEnv->env_id;	//Always use page #: 800000, 801000, 803000

	//cprintf("testing...\n");
	{
		if (sys_calculate_modified_frames() != 3)
		{
			///*sys_env_destroy(envIdSlave);*/
			panic("Modified frames not removed from list (or not updated) correctly when the modified list reaches MAX");
		}

		if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  0)
		{
			///*sys_env_destroy(envIdSlave);*/
			panic("Unexpected extra/less pages have been added to page file.. NOT Expected to add new pages to the page file");
		}
		if( arr[0] != -1) 						{/*sys_env_destroy(envIdSlave);*/panic("modified page not updated on page file OR not reclaimed correctly");}
		if( arr[PAGE_SIZE * 1024 * 1] != -1) 	{/*sys_env_destroy(envIdSlave);*/panic("modified page not updated on page file OR not reclaimed correctly");}
		if( arr[PAGE_SIZE * 1024 * 2] != -1) 	{/*sys_env_destroy(envIdSlave);*/panic("modified page not updated on page file OR not reclaimed correctly");}
		if( arr[PAGE_SIZE * 1024 * 3] != -1) 	{/*sys_env_destroy(envIdSlave);*/panic("modified page not updated on page file OR not reclaimed correctly");}
		if( arr[PAGE_SIZE * 1024 * 4] != -1) 	{/*sys_env_destroy(envIdSlave);*/panic("modified page not updated on page file OR not reclaimed correctly");}
		if( arr[PAGE_SIZE * 1024 * 5] != -1) 	{/*sys_env_destroy(envIdSlave);*/panic("modified page not updated on page file OR not reclaimed correctly");}
		if( arr[PAGE_SIZE * 1024 * 6] != -1) 	{/*sys_env_destroy(envIdSlave);*/panic("modified page not updated on page file OR not reclaimed correctly");}
		if( arr[PAGE_SIZE * 1024 * 7] != -1) 	{/*sys_env_destroy(envIdSlave);*/panic("modified page not updated on page file OR not reclaimed correctly");}
		if( arr[0] != -1) 						{/*sys_env_destroy(envIdSlave);*/panic("modified page not updated on page file OR not reclaimed correctly");}

		if (sys_calculate_modified_frames() != 0) {/*sys_env_destroy(envIdSlave);*/panic("Modified frames not removed from list (or isModified/modified bit is not updated) correctly when the modified list reaches MAX %d",sys_calculate_modified_frames());}
	}

	return;
}
