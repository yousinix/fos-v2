// Simple command-line kernel prompt useful for
// controlling the kernel and exploring the system interactively.

/*
KEY WORDS
==========
CONSTANTS:	WHITESPACE, NUM_OF_COMMANDS
VARIABLES:	Command, commands, name, description, function_to_execute, number_of_arguments, arguments, command_string, command_line, command_found
FUNCTIONS:	readline, cprintf, execute_command, run_command_prompt, command_kernel_info, command_help, strcmp, strsplit, start_of_kernel, start_of_uninitialized_data_section, end_of_kernel_code_section, end_of_kernel
=====================================================================================================================================================================================================
 */

#include <inc/stdio.h>
#include <inc/string.h>
#include <inc/memlayout.h>
#include <inc/assert.h>
#include <inc/x86.h>
#include <inc/disk.h>


#include <kern/console.h>
#include <kern/command_prompt.h>
#include <kern/memory_manager.h>
#include <kern/trap.h>
#include <kern/kdebug.h>
#include <kern/user_environment.h>
#include <kern/file_manager.h>
#include <kern/sched.h>
#include <kern/kheap.h>
#include <kern/utilities.h>
#include <kern/priority_manager.h>


//Structure for each command
struct Command
{
	char *name;
	char *description;
	// return -1 to force command prompt to exit
	int (*function_to_execute)(int number_of_arguments, char** arguments);
};

//Functions Declaration
extern uint32 enableBuffering();
extern uint32 isBufferingEnabled();
extern void setModifiedBufferLength(uint32 length) ;
extern uint32 getModifiedBufferLength();
extern int test_krealloc();
extern int test_krealloc_BF();

int execute_command(char *command_string);
int command_writeusermem(int number_of_arguments, char **arguments);
int command_readusermem(int number_of_arguments, char **arguments);
int command_readuserblock(int number_of_arguments, char **arguments);
int command_remove_table(int number_of_arguments, char **arguments);
int command_allocuserpage(int number_of_arguments, char **arguments);
int command_meminfo(int number_of_arguments, char **arguments);

int command_set_page_rep_FIFO(int number_of_arguments, char **arguments);
int command_set_page_rep_CLOCK(int number_of_arguments, char **arguments);
int command_set_page_rep_LRU(int number_of_arguments, char **arguments);
int command_set_page_rep_ModifiedCLOCK(int number_of_arguments, char **arguments);
int command_print_page_rep(int number_of_arguments, char **arguments);

int command_set_uheap_plac_FIRSTFIT(int number_of_arguments, char **arguments);
int command_set_uheap_plac_BESTFIT(int number_of_arguments, char **arguments);
int command_set_uheap_plac_NEXTFIT(int number_of_arguments, char **arguments);
int command_set_uheap_plac_WORSTFIT(int number_of_arguments, char **arguments);
int command_print_uheap_plac(int number_of_arguments, char **arguments);

int command_set_kheap_plac_CONTALLOC(int number_of_arguments, char **arguments);
int command_set_kheap_plac_FIRSTFIT(int number_of_arguments, char **arguments);
int command_set_kheap_plac_BESTFIT(int number_of_arguments, char **arguments);
int command_set_kheap_plac_NEXTFIT(int number_of_arguments, char **arguments);
int command_set_kheap_plac_WORSTFIT(int number_of_arguments, char **arguments);
int command_print_kheap_plac(int number_of_arguments, char **arguments);

int command_disable_modified_buffer(int number_of_arguments, char **arguments);
int command_enable_modified_buffer(int number_of_arguments, char **arguments);

//2016
int command_disable_buffering(int number_of_arguments, char **arguments);
int command_enable_buffering(int number_of_arguments, char **arguments);

int command_set_modified_buffer_length(int number_of_arguments, char **arguments);
int command_get_modified_buffer_length(int number_of_arguments, char **arguments);

//2016: Kernel Heap Tests
extern int test_kmalloc();
extern int test_kmalloc_nextfit();
extern int test_kmalloc_bestfit1();
extern int test_kmalloc_bestfit2();
extern int test_kmalloc_firstfit1();
extern int test_kmalloc_firstfit2();
extern int test_kmalloc_worstfit();
extern int test_kfree();
extern int test_kfree_bestfit();
extern int test_kheap_phys_addr();
extern int test_kheap_virt_addr();
extern int test_three_creation_functions();
extern void test_priority_normal_and_higher();
extern void test_priority_normal_and_lower();
extern int test_kfreeall();
extern int test_kexpand();
extern int test_kshrink();
extern int test_kfreelast();
extern int test_krealloc();

int command_test_kmalloc(int number_of_arguments, char **arguments);
int command_test_kfree(int number_of_arguments, char **arguments);
int command_test_kheap_phys_addr(int number_of_arguments, char **arguments);
int command_test_kheap_virt_addr(int number_of_arguments, char **arguments);
int command_test_three_creation_functions(int number_of_arguments, char **arguments);
int command_test_kfreeall(int number_of_arguments, char **arguments);
int command_test_kexpand(int number_of_arguments, char **arguments);
int command_test_kshrink(int number_of_arguments, char **arguments);
int command_test_kfreelast(int number_of_arguments, char **arguments);
int command_test_krealloc(int number_of_arguments, char **arguments);
int command_test_sc4_MLFQ(int number_of_arguments, char **arguments);

//2018
int command_sch_RR(int number_of_arguments, char **arguments);
int command_sch_MLFQ(int number_of_arguments, char **arguments);
int command_print_sch_method(int number_of_arguments, char **arguments);
int command_sch_test(int number_of_arguments, char **arguments);

int command_test_priority1(int number_of_arguments, char **arguments);
int command_test_priority2(int number_of_arguments, char **arguments);

//Array of commands. (initialized)
struct Command commands[] =
{
		{ "help", "Display this list of commands", command_help },
		{ "kernel_info", "Display information about the kernel", command_kernel_info },
		{ "wum", "writes one byte to specific location in given environment" ,command_writeusermem},
		{ "rum", "reads one byte from specific location in given environment" ,command_readusermem},
		{ "wm", "writes one byte to specific location" ,command_writemem_k},
		{ "rm", "reads one byte from specific location" ,command_readmem_k},
		{ "rub", "reads block of bytes from specific location in given environment" ,command_readuserblock},
		{ "kill", "kill the given environment (by its ID) from the system", command_kill_program},
		{ "rut", "", command_remove_table},
		{ "aup", "", command_allocuserpage},
		{ "meminfo", "", command_meminfo},

		{ "schedMLFQ", "switch the scheduler to MLFQ with given # queues & quantums", command_sch_MLFQ},
		{ "schedRR", "switch the scheduler to RR with given quantum", command_sch_RR},
		{"sched?", "print current scheduler algorithm", command_print_sch_method},
		{"schedTest", "Used for turning on/off the scheduler test", command_sch_test},

		{ "run", "runs a single user program", command_run_program },
		{"load", "load a single user program to mem with status = NEW", commnad_load_env},
		{"runall", "run all loaded programs", command_run_all},
		{"printall", "print all loaded programs", command_print_all},
		{"killall", "kill all environments in the system", command_kill_all},
		{"lru", "set replacement algorithm to LRU", command_set_page_rep_LRU},
		{"fifo", "set replacement algorithm to FIFO", command_set_page_rep_FIFO},
		{"clock", "set replacement algorithm to CLOCK", command_set_page_rep_CLOCK},
		{"modifiedclock", "set replacement algorithm to modified CLOCK", command_set_page_rep_ModifiedCLOCK},
		{"rep?", "print current replacement algorithm", command_print_page_rep},

		{"uhfirstfit", "set USER heap placement strategy to FIRST FIT", command_set_uheap_plac_FIRSTFIT},
		{"uhbestfit", "set USER heap placement strategy to BEST FIT", command_set_uheap_plac_BESTFIT},
		{"uhnextfit", "set USER heap placement strategy to NEXT FIT", command_set_uheap_plac_NEXTFIT},
		{"uhworstfit", "set USER heap placement strategy to WORST FIT", command_set_uheap_plac_WORSTFIT},
		{"uheap?", "print current USER heap placement strategy", command_print_uheap_plac},

		{"khcontalloc", "set KERNEL heap placement strategy to CONTINUOUS ALLOCATION", command_set_kheap_plac_CONTALLOC},
		{"khfirstfit", "set KERNEL heap placement strategy to FIRST FIT", command_set_kheap_plac_FIRSTFIT},
		{"khbestfit", "set KERNEL heap placement strategy to BEST FIT", command_set_kheap_plac_BESTFIT},
		{"khnextfit", "set KERNEL heap placement strategy to NEXT FIT", command_set_kheap_plac_NEXTFIT},
		{"khworstfit", "set KERNEL heap placement strategy to WORST FIT", command_set_kheap_plac_WORSTFIT},
		{"kheap?", "print current KERNEL heap placement strategy", command_print_kheap_plac},

		//2016
		{"nobuff", "", command_disable_buffering},
		{"buff", "", command_enable_buffering},

		{"nomodbuff", "", command_disable_modified_buffer},
		{"modbuff", "", command_enable_modified_buffer},

		{"modbufflength?", "", command_get_modified_buffer_length},
		{"modbufflength", "", command_set_modified_buffer_length},

		{"tstkmalloc", "Kernel Heap: test kmalloc (return address, size, mem access...etc)", command_test_kmalloc},
		{"tstkfree", "Kernel Heap: test kfree (freed frames, mem access...etc)", command_test_kfree},
		{"tstkphysaddr", "Kernel Heap: test kheap_phys_addr", command_test_kheap_phys_addr},
		{"tstkvirtaddr", "Kernel Heap: test kheap_virt_addr", command_test_kheap_virt_addr},
		{"tst3functions", "Env Load: test the creation of new dir, tables and pages WS", command_test_three_creation_functions},
		{"tstkfreeall", "Kernel Heap: test kfreeall (freed frames, mem access...etc)", command_test_kfreeall},
		{"tstkexpand", "Kernel Heap: test expanding last allocated var", command_test_kexpand},
		{"tstkshrink", "Kernel Heap: test srinking last allocated var", command_test_kshrink},
		{"tstkfreelast", "Kernel Heap: test freeing last allocated var", command_test_kfreelast},
		{"tstkrealloc","Kernel realloc: test realloc (virtual address = 0)",command_test_krealloc },
		{"tstpriority1", "Tests the priority of the program (Normal and Higher)", command_test_priority1},
		{"tstpriority2", "Tests the priority of the program (Normal and Lower)", command_test_priority2}

};

//Number of commands = size of the array / size of command structure
#define NUM_OF_COMMANDS (sizeof(commands)/sizeof(commands[0]))

unsigned read_eip();

// ********** This DosKey supported readline function is implemented by **********
// ********** Abdullah Najuib ( FCIS T.A.), 3rd year student, FCIS, 2012

#define BUFLEN 1024
//#define CMD_NUMBER sizeof(comds)/sizeof(comds[0])

#define WHITESPACE "\t\r\n "
#define HISTORY_MAX 19
int last_command_idx = -1;
char command_history[HISTORY_MAX+1][BUFLEN];
char empty[BUFLEN];

void clearandwritecommand(int* i, int commandidx, char* buf, int *last_index) {
	for (int j = 0; j < *i; j++) {
		cputchar('\b');
	}
	int len = strlen(command_history[commandidx]);
	memcpy(buf, empty, BUFLEN);
	for (*i = 0; *i < len; (*i)++) {
		cputchar(command_history[commandidx][*i]);
		buf[*i] = command_history[commandidx][*i];
	}
	*last_index = len;
}

void RoundAutoCompleteCommandWithTheSamePrefix(int old_buf_len, char* prefix_element,
		char* buf, int* i, int *last_index) {
	for (int j = 0; j < old_buf_len; j++) {
		cputchar('\b');
	}
	int len = strlen(prefix_element);
	memcpy(buf, empty, BUFLEN);
	for (*i = 0; *i < len; (*i)++) {
		cputchar(prefix_element[*i]);
		buf[*i] = prefix_element[*i];
	}
	*last_index = len;
}

char PrefixList[100][1024];
void clear_prefix_list()
{
	for (int i = 0; i < 100; ++i) {
		memset(PrefixList[i], 0, 1024);}
}

void command_prompt_readline(const char *prompt, char* buf) {
	int i, c, echoing, lastIndex;
	if (prompt != NULL)
		cprintf("%s", prompt);

	int commandidx = last_command_idx + 1;
	int prefix_list_idx = lastIndex = i = 0;
	int prefix_list_size, last_c;
	echoing = iscons(0);
	while (1) {
		c = getchar();
		if (i > lastIndex)
			lastIndex = i;
		if (c < 0) {

			if (c != -E_EOF)
				cprintf("read error: %e\n", c);
			return;
		} else if (c == 226) { // Up arrow
			if (commandidx)
				commandidx--;
			clearandwritecommand(&i, commandidx, buf, &lastIndex);
		} else if (c == 227) { // Down arrow
			if (commandidx < last_command_idx)
				commandidx++;
			if (last_command_idx >= 0)
				clearandwritecommand(&i, commandidx, buf, &lastIndex);
		} else if (c == 9) { // Tab button
			if (last_c != 9) {
				clear_prefix_list(PrefixList, 100);
				if (strlen(buf) == 0 || last_c == 255)
					continue;
				char *arguments[MAX_ARGUMENTS];
				int number_of_arguments = prefix_list_size = 0;
				char temp_buf[1024];
				strcpy(temp_buf, buf);
				int bufLength = strlen(buf);
				if (buf[bufLength - 1] == ' ')
					continue;
				strsplit(temp_buf, WHITESPACE, arguments, &number_of_arguments);
				int it_str = 0;
				if (number_of_arguments > 1) {
					if((strcmp(arguments[0], "run") != 0) && (strcmp(arguments[0], "load") != 0)) // to autocomplete only in case that the command take arguments and defined arguments (run & load) only
						continue;
					char temp[1024] = "";
					int TotalLen = bufLength - strlen(arguments[number_of_arguments - 1]);
					for (int var = 0; var < TotalLen; ++var) {
						temp[it_str++] = buf[var];
					}
					strcpy(buf, temp);   //buf contains all arguments except the last one
					strcpy(temp_buf, arguments[number_of_arguments - 1]);   //temp_buf contains the last argument
				}
				int it_prefix_list = 0;
				if(number_of_arguments == 1)
				{
					for (int var = 0; var < NUM_OF_COMMANDS; ++var) {
						int x = strncmp(temp_buf, commands[var].name, strlen(temp_buf));
						if (x == 0) {
							it_str = -1;
							char string[1024] = "";
							for (int var3 = 0; var3 < strlen(commands[var].name); ++var3) {
								string[++it_str] = commands[var].name[var3];
							}
							memset(PrefixList[it_prefix_list], 0, 1024);
							strncpy(PrefixList[it_prefix_list], string, it_str + 1);
							it_prefix_list++;
						}
					}
				}
				else
				{
					for (int var = 0; var < NUM_USER_PROGS; ++var) {
						int x = strncmp(temp_buf, ptr_UserPrograms[var].name, strlen(temp_buf));
						if (x == 0) {
							it_str = -1;
							char string[1024] = "";
							if (number_of_arguments > 1) {
								for (int var2 = 0; var2 < strlen(buf); ++var2) {
									string[++it_str] = buf[var2];
								}
							}
							for (int var3 = 0; var3 < strlen(ptr_UserPrograms[var].name) ; ++var3) {
								string[++it_str] = ptr_UserPrograms[var].name[var3];
							}
							memset(PrefixList[it_prefix_list], 0, 1024);
							strncpy(PrefixList[it_prefix_list], string, it_str + 1);
							it_prefix_list++;
						}
					}
				}
				prefix_list_size = it_prefix_list;
				if (it_prefix_list) {
					prefix_list_idx = it_str = 0;
					for (int var2 = 0; var2 < strlen(PrefixList[0]); ++var2) {
						buf[it_str++] = PrefixList[0][var2];}
					for (int var = 0; var < bufLength; ++var) {
						cputchar('\b');}
					for (int j = 0; j < strlen(buf); ++j) {
						cputchar(buf[j]);}
					i = lastIndex = strlen(buf);
				}
			}
			else {
				if (prefix_list_size > 0) {	int prev = prefix_list_idx;
				prefix_list_idx = (prefix_list_idx + 1) % prefix_list_size;
				RoundAutoCompleteCommandWithTheSamePrefix(strlen(PrefixList[prev]), PrefixList[prefix_list_idx], buf, &i, &lastIndex);
				}
			}
		}

		else if (c == 228) { // left arrow
			if (i > 0) {
				i--;
				cputchar(c);
			}
		} else if (c == 229) { // right arrow
			if (i < lastIndex) {
				i++;
				cputchar(c);
			}
		}
		else if (c == 0xE9 && i > 0) {		 // KEY_DEL
			for (int var = i; var <= lastIndex; ++var) {
				buf[var] = buf[var + 1];
			}
			lastIndex--;
		}
		else if (c >= ' ' && i < BUFLEN - 1 && c != 229 && c != 228) {
			if (echoing)
				cputchar(c);
			buf[i++] = c;
			lastIndex++;
		} else if (c == '\b' && i > 0) {

			if (echoing)
				cputchar(c);
			for (int var = i; var <= i; ++var) {
				buf[var - 1] = buf[var];
			}
			i--;
		} else if (c == '\n' || c == '\r') {

			if (echoing)
				cputchar(c);

			buf[lastIndex] = 0;
			if (last_command_idx == HISTORY_MAX) {
				for (int idx = 0; idx < HISTORY_MAX; idx++) {
					memcpy(command_history[idx], command_history[idx + 1],
							BUFLEN);
				}
				memcpy(command_history[HISTORY_MAX], buf, BUFLEN);
			} else if (strcmp(command_history[last_command_idx], buf) != 0) {
				memcpy(command_history[++last_command_idx], buf, BUFLEN);
			}
			return;

		}
		last_c = c;
	}
}
// ******************************************************************
// ******************************************************************

//invoke the command prompt
void run_command_prompt()
{
	char command_line[BUFLEN];

	while (1==1)
	{
		//readline("FOS> ", command_line);

		// ********** This DosKey supported readline function is a combined implementation from **********
		// ********** 		Mohamed Raafat & Mohamed Yousry, 3rd year students, FCIS, 2017		**********
		// ********** 				Combined, edited and modified by TA\Ghada Hamed				**********
		memset(command_line, 0, sizeof(command_line));
		command_prompt_readline("FOS> ", command_line);


		//parse and execute the command
		if (command_line != NULL)
			if (execute_command(command_line) < 0)
				break;
	}
}

/***** Kernel command prompt command interpreter *****/

//define the white-space symbols
#define WHITESPACE "\t\r\n "

//Function to parse any command and execute it
//(simply by calling its corresponding function)
int execute_command(char *command_string)
{
	// Split the command string into whitespace-separated arguments
	int number_of_arguments;
	//allocate array of char * of size MAX_ARGUMENTS = 16 found in string.h
	char *arguments[MAX_ARGUMENTS];


	strsplit(command_string, WHITESPACE, arguments, &number_of_arguments) ;
	if (number_of_arguments == 0)
		return 0;

	// Lookup in the commands array and execute the command
	int command_found = 0;
	int i ;
	for (i = 0; i < NUM_OF_COMMANDS; i++)
	{
		if (strcmp(arguments[0], commands[i].name) == 0)
		{
			command_found = 1;
			break;
		}
	}

	if(command_found)
	{
		int return_value;
		return_value = commands[i].function_to_execute(number_of_arguments, arguments);
		return return_value;
	}
	else
	{
		//if not found, then it's unknown command
		cprintf("Unknown command '%s'\n", arguments[0]);
		return 0;
	}
}

/***** Implementations of basic kernel command prompt commands *****/

//print name and description of each command
int command_help(int number_of_arguments, char **arguments)
{
	int i;
	for (i = 0; i < NUM_OF_COMMANDS; i++)
		cprintf("%s - %s\n", commands[i].name, commands[i].description);

	cprintf("-------------------\n");

	for (i = 0; i < NUM_USER_PROGS; i++)
		cprintf("run %s - %s [User Program]\n", ptr_UserPrograms[i].name, ptr_UserPrograms[i].desc);
	return 0;
}

//print information about kernel addresses and kernel size
int command_kernel_info(int number_of_arguments, char **arguments )
{
	extern char start_of_kernel[], end_of_kernel_code_section[], start_of_uninitialized_data_section[], end_of_kernel[];

	cprintf("Special kernel symbols:\n");
	cprintf("  Start Address of the kernel 			%08x (virt)  %08x (phys)\n", start_of_kernel, start_of_kernel - KERNEL_BASE);
	cprintf("  End address of kernel code  			%08x (virt)  %08x (phys)\n", end_of_kernel_code_section, end_of_kernel_code_section - KERNEL_BASE);
	cprintf("  Start addr. of uninitialized data section 	%08x (virt)  %08x (phys)\n", start_of_uninitialized_data_section, start_of_uninitialized_data_section - KERNEL_BASE);
	cprintf("  End address of the kernel   			%08x (virt)  %08x (phys)\n", end_of_kernel, end_of_kernel - KERNEL_BASE);
	cprintf("Kernel executable memory footprint: %d KB\n",
			(end_of_kernel-start_of_kernel+1023)/1024);
	return 0;
}

int command_writeusermem(int number_of_arguments, char **arguments)
{
	int32 envId = strtol(arguments[1],NULL, 10);
	struct Env* env = NULL;
	envid2env(envId, &env, 0 );

	int address = strtol(arguments[3], NULL, 16);

	if(env == NULL) return 0;

	uint32 oldDir = rcr3();
	//lcr3((uint32) K_PHYSICAL_ADDRESS( env->env_pgdir));
	lcr3((uint32) (env->env_cr3));

	unsigned char *ptr = (unsigned char *)(address) ;

	//Write the given Character
	*ptr = arguments[2][0];
	lcr3(oldDir);

	return 0;
}

int command_writemem_k(int number_of_arguments, char **arguments)
{
	unsigned char* address = (unsigned char*)strtol(arguments[1], NULL, 16)+KERNEL_BASE;
	int size = strtol(arguments[2], NULL, 10);
	int c, i=0;
	int stringLen = strlen(arguments[3]);

	for(c=0; c<size; c++)
	{
		for(i=0;i < stringLen; i++)
		{
			*address = arguments[3][i];
			address++;
		}
	}
	return 0;


}

int command_readusermem(int number_of_arguments, char **arguments)
{
	int32 envId = strtol(arguments[1],NULL, 10);
	struct Env* env = NULL;
	envid2env(envId, &env, 0 );

	int address = strtol(arguments[2], NULL, 16);

	if(env == NULL) return 0;

	uint32 oldDir = rcr3();
	//lcr3((uint32) K_PHYSICAL_ADDRESS( env->env_pgdir));
	lcr3((uint32)( env->env_cr3));

	unsigned char *ptr = (unsigned char *)(address) ;

	//Write the given Character
	cprintf("value at address %x = %c\n", address, *ptr);

	lcr3(oldDir);
	return 0;
}

int command_readmem_k(int number_of_arguments, char **arguments)
{
	unsigned char* address = (unsigned char*)strtol(arguments[1], NULL, 16)+KERNEL_BASE;
	int size = strtol(arguments[2], NULL, 10);
	int i=0;
	for(;i < size; i++)
	{
		cprintf("%c",*address++);
	}
	cprintf("\n");
	return 0;
}


int command_readuserblock(int number_of_arguments, char **arguments)
{
	int32 envId = strtol(arguments[1],NULL, 10);
	struct Env* env = NULL;
	envid2env(envId, &env, 0 );

	int address = strtol(arguments[2], NULL, 16);
	int nBytes = strtol(arguments[3], NULL, 10);

	unsigned char *ptr = (unsigned char *)(address) ;
	//Write the given Character

	if(env == NULL) return 0;

	uint32 oldDir = rcr3();
	//lcr3((uint32) K_PHYSICAL_ADDRESS( env->env_pgdir));
	lcr3((uint32)( env->env_cr3));

	int i;
	for(i = 0;i<nBytes; i++)
	{
		cprintf("%08x : %02x  %c\n", ptr, *ptr, *ptr);
		ptr++;
	}
	lcr3(oldDir);

	return 0;
}

int command_remove_table(int number_of_arguments, char **arguments)
{
	int32 envId = strtol(arguments[1],NULL, 10);
	struct Env* env = NULL;
	envid2env(envId, &env, 0 );
	if(env == 0) return 0;

	uint32 address = strtol(arguments[2], NULL, 16);
	unsigned char *va = (unsigned char *)(address) ;
	uint32 table_pa = env->env_page_directory[PDX(address)] & 0xFFFFF000;

	//remove the table
	if(USE_KHEAP && !CHECK_IF_KERNEL_ADDRESS(va))
	{
		kfree((void*)kheap_virtual_address(table_pa));
	}
	else
	{
		// get the physical address and Frame_Info of the page table
		struct Frame_Info *table_frame_info = to_frame_info(table_pa);
		// set references of the table frame to 0 then free it by adding
		// to the free frame list
		table_frame_info->references = 0;
		free_frame(table_frame_info);
	}

	// set the corresponding entry in the directory to 0
	uint32 dir_index = PDX(va);
	env->env_page_directory[dir_index] &= (~PERM_PRESENT);
	tlbflush();
	return 0;
}

int command_allocuserpage(int number_of_arguments, char **arguments)
{
	int32 envId = strtol(arguments[1],NULL, 10);
	struct Env* env = NULL;
	envid2env(envId, &env, 0 );
	if(env == 0) return 0;

	uint32 address = strtol(arguments[2], NULL, 16);
	unsigned char *va = (unsigned char *)(address) ;

	// Allocate a single frame from the free frame list
	struct Frame_Info * ptr_frame_info ;
	int ret = allocate_frame(&ptr_frame_info);
	if (ret == E_NO_MEM)
	{
		cprintf("ERROR: no enough memory\n");
		return 0;
	}

	// Map this frame to the given user virtual address
	map_frame(env->env_page_directory, ptr_frame_info, va, PERM_WRITEABLE | PERM_USER);

	return 0;
}

int command_meminfo(int number_of_arguments, char **arguments)
{
	struct freeFramesCounters counters =calculate_available_frames();
	cprintf("Total available frames = %d\nFree Buffered = %d\nFree Not Buffered = %d\nModified = %d\n",
			counters.freeBuffered+ counters.freeNotBuffered+ counters.modified, counters.freeBuffered, counters.freeNotBuffered, counters.modified);

	cprintf("Num of calls for kheap_virtual_address [in last run] = %d\n", numOfKheapVACalls);

	return 0;
}

//2020
struct Env * CreateEnv(int number_of_arguments, char **arguments)
{
	struct Env* env;
	uint32 pageWSSize = __PWS_MAX_SIZE;		//arg#3 default
	uint32 LRUSecondListSize = 0;			//arg#4 default
	uint32 percent_WS_pages_to_remove = 0;	//arg#5 default
//#if USE_KHEAP
	{
		switch (number_of_arguments)
		{
		case 5:
			if(!isPageReplacmentAlgorithmLRU(PG_REP_LRU_LISTS_APPROX))
			{
				cprintf("ERROR: Current Replacement is NOT LRU LISTS, invalid number of args\nUsage: <command> <prog_name> <page_WS_size> [<LRU_second_list_size>] [<DYN_LOC_SCOPE_percent_WS_to_remove>]\naborting...\n");
				return NULL;
			}
			percent_WS_pages_to_remove = strtol(arguments[4], NULL, 10);
			LRUSecondListSize = strtol(arguments[3], NULL, 10);
			pageWSSize = strtol(arguments[2], NULL, 10);
			break;
		case 4:
			if(!isPageReplacmentAlgorithmLRU(PG_REP_LRU_LISTS_APPROX))
			{
				percent_WS_pages_to_remove = strtol(arguments[3], NULL, 10);
			}
			else
			{
				LRUSecondListSize = strtol(arguments[3], NULL, 10);
			}
			pageWSSize = strtol(arguments[2], NULL, 10);
			break;
		case 3:
			if(isPageReplacmentAlgorithmLRU(PG_REP_LRU_LISTS_APPROX))
			{
				cprintf("ERROR: Current Replacement is LRU LISTS, Please specify a working set size in the 3rd arg and LRU second list size in the 4th arg, aborting.\n");
				return NULL;
			}
			pageWSSize = strtol(arguments[2], NULL, 10);
			break;
		default:
			cprintf("ERROR: invalid number of args\nUsage: <command> <prog_name> <page_WS_size> [<LRU_second_list_size>] [<DYN_LOC_SCOPE_percent_WS_to_remove>]\naborting...\n");
			return NULL;

			break;
		}
		if(pageWSSize > __PWS_MAX_SIZE)
		{
			cprintf("ERROR: size of WS must be less than or equal to %d... aborting", __PWS_MAX_SIZE);
			return NULL;
		}
		if(isPageReplacmentAlgorithmLRU(PG_REP_LRU_LISTS_APPROX))
		{
			if (LRUSecondListSize > pageWSSize - 1)
			{
				cprintf("ERROR: size of LRU second list can't equal/exceed the size of the page WS... aborting\n");
				return NULL;
			}
		}
		assert(percent_WS_pages_to_remove >= 0 && percent_WS_pages_to_remove <= 100);

	}
//#else
//	{
//		switch (number_of_arguments)
//		{
//		case 3:
//			percent_WS_pages_to_remove = strtol(arguments[2], NULL, 10);
//		case 2:
//			break;
//		default:
//			cprintf("ERROR: invalid number of args\nUsage: <command> <prog_name> [<DYN_LOC_SCOPE_percent_WS_to_remove>]\naborting...\n");
//			return NULL;
//
//			break;
//		}
//		if(isPageReplacmentAlgorithmLRU(PG_REP_LRU_LISTS_APPROX))
//		{
//			LRUSecondListSize = __LRU_SNDLST_SIZE;
//		}
//	}
//#endif
	assert(percent_WS_pages_to_remove >= 0 && percent_WS_pages_to_remove <= 100);
	env = env_create(arguments[1], pageWSSize, LRUSecondListSize, percent_WS_pages_to_remove);

	return env;
}

int command_run_program(int number_of_arguments, char **arguments)
{
	struct Env *env = CreateEnv(number_of_arguments, arguments);

	if(env == NULL) return 0;
	cprintf("\nEnvironment Id= %d\n",env->env_id);

	numOfKheapVACalls = 0;

	sched_new_env(env);
	sched_run_env(env->env_id);

	return 0;
}

int command_kill_program(int number_of_arguments, char **arguments)
{
	int32 envId = strtol(arguments[1],NULL, 10);

	sched_kill_env(envId);

	return 0;
}

int commnad_load_env(int number_of_arguments, char **arguments)
{
	struct Env *env = CreateEnv(number_of_arguments, arguments);
	if (env == NULL)
		return 0 ;

	sched_new_env(env) ;

	cprintf("\nEnvironment Id= %d\n",env->env_id);
	return 0;
}

int command_run_all(int number_of_arguments, char **arguments)
{
	numOfKheapVACalls = 0;
	sched_run_all();

	return 0 ;
}

int command_print_all(int number_of_arguments, char **arguments)
{
	sched_print_all();

	return 0 ;
}

int command_kill_all(int number_of_arguments, char **arguments)
{
	sched_kill_all();

	return 0 ;
}

int command_set_page_rep_LRU(int number_of_arguments, char **arguments)
{
	if (number_of_arguments < 2)
	{
		cprintf("ERROR: please specify the LRU Approx Type (1: TimeStamp Approx, 2: Lists Approx), aborting...\n");
		return 0;
	}
	int LRU_TYPE = strtol(arguments[1], NULL, 10) ;
	if (LRU_TYPE == PG_REP_LRU_TIME_APPROX)
	{
		setPageReplacmentAlgorithmLRU(LRU_TYPE);
		cprintf("Page replacement algorithm is now LRU with TimeStamp approximation\n");
	}
	else if (LRU_TYPE == PG_REP_LRU_LISTS_APPROX)
	{
		setPageReplacmentAlgorithmLRU(LRU_TYPE);
		cprintf("Page replacement algorithm is now LRU with LISTS approximation\n");
	}
	else
	{
		cprintf("ERROR: Invalid LRU Approx Type (1: TimeStamp Approx, 2: Lists Approx), aborting...\n");
		return 0;
	}
	return 0;
}

int command_set_page_rep_CLOCK(int number_of_arguments, char **arguments)
{
	setPageReplacmentAlgorithmCLOCK();
	cprintf("Page replacement algorithm is now CLOCK\n");
	return 0;
}

int command_set_page_rep_FIFO(int number_of_arguments, char **arguments)
{
	setPageReplacmentAlgorithmFIFO();
	cprintf("Page replacement algorithm is now FIFO\n");
	return 0;
}

int command_set_page_rep_ModifiedCLOCK(int number_of_arguments, char **arguments)
{
	setPageReplacmentAlgorithmModifiedCLOCK();
	cprintf("Page replacement algorithm is now Modified CLOCK\n");
	return 0;
}

/*2018*///BEGIN======================================================
int command_sch_RR(int number_of_arguments, char **arguments)
{
	uint8 quantum = strtol(arguments[1], NULL, 10);

	sched_init_RR(quantum);
	cprintf("Scheduler is now set to Round Robin with quantum %d ms\n", quantums[0]);
	return 0;
}
int command_sch_MLFQ(int number_of_arguments, char **arguments)
{
	uint8 numOfLevels = strtol(arguments[1], NULL, 10);
	uint8 quantumOfEachLevel[MAX_ARGUMENTS - 2] ;
	for (int i = 2 ; i < number_of_arguments ; i++)
	{
		quantumOfEachLevel[i-2] = strtol(arguments[i], NULL, 10);
	}

	sched_init_MLFQ(numOfLevels, quantumOfEachLevel);

	cprintf("Scheduler is now set to MLFQ with quantums: ");
	for (int i = 0 ; i < num_of_ready_queues; i++)
	{
		cprintf("%d   ", quantums[i]) ;
	}
	cprintf("\n");
	return 0;
}
int command_print_sch_method(int number_of_arguments, char **arguments)
{
	if (isSchedMethodMLFQ())
	{
		cprintf("Current scheduler method is MLFQ with quantums: ");
		for (int i = 0 ; i < num_of_ready_queues; i++)
		{
			cprintf("%d   ", quantums[i]) ;
		}
		cprintf("\n");
	}
	else if (isSchedMethodRR())
	{
		cprintf("Current scheduler method is Round Robin with quantum %d ms\n", quantums[0]);
	}

	else
		cprintf("Current scheduler method is UNDEFINED\n");

	return 0;
}
int command_sch_test(int number_of_arguments, char **arguments)
{
	int status  = strtol(arguments[1], NULL, 10);
	chksch(status);
	if (status == 0)
		cprintf("Testing the scheduler is TURNED OFF\n");
	else if (status == 1)
		cprintf("Testing the scheduler is TURNED ON\n");
	return 0;
}

/*2018*///END======================================================


/*2015*///BEGIN======================================================
int command_print_page_rep(int number_of_arguments, char **arguments)
{
	if (isPageReplacmentAlgorithmCLOCK())
		cprintf("Page replacement algorithm is CLOCK\n");
	else if (isPageReplacmentAlgorithmLRU(PG_REP_LRU_TIME_APPROX))
		cprintf("Page replacement algorithm is LRU with TimeStamp approximation\n");
	else if (isPageReplacmentAlgorithmLRU(PG_REP_LRU_LISTS_APPROX))
		cprintf("Page replacement algorithm is LRU with LISTS approximation\n");
	else if (isPageReplacmentAlgorithmFIFO())
		cprintf("Page replacement algorithm is FIFO\n");
	else if (isPageReplacmentAlgorithmModifiedCLOCK())
		cprintf("Page replacement algorithm is Modified CLOCK\n");
	else
		cprintf("Page replacement algorithm is UNDEFINED\n");

	return 0;
}


int command_set_uheap_plac_FIRSTFIT(int number_of_arguments, char **arguments)
{
	setUHeapPlacementStrategyFIRSTFIT();
	cprintf("User Heap placement strategy is now FIRST FIT\n");
	return 0;
}

int command_set_uheap_plac_BESTFIT(int number_of_arguments, char **arguments)
{
	setUHeapPlacementStrategyBESTFIT();
	cprintf("User Heap placement strategy is now BEST FIT\n");
	return 0;
}

int command_set_uheap_plac_NEXTFIT(int number_of_arguments, char **arguments)
{
	setUHeapPlacementStrategyNEXTFIT();
	cprintf("User Heap placement strategy is now NEXT FIT\n");
	return 0;
}
int command_set_uheap_plac_WORSTFIT(int number_of_arguments, char **arguments)
{
	setUHeapPlacementStrategyWORSTFIT();
	cprintf("User Heap placement strategy is now WORST FIT\n");
	return 0;
}

int command_print_uheap_plac(int number_of_arguments, char **arguments)
{
	if (isUHeapPlacementStrategyFIRSTFIT())
		cprintf("User Heap placement strategy is FIRST FIT\n");
	else if (isUHeapPlacementStrategyBESTFIT())
		cprintf("User Heap placement strategy is BEST FIT\n");
	else if (isUHeapPlacementStrategyNEXTFIT())
		cprintf("User Heap placement strategy is NEXT FIT\n");
	else if (isUHeapPlacementStrategyWORSTFIT())
		cprintf("User Heap placement strategy is WORST FIT\n");
	else
		cprintf("User Heap placement strategy is UNDEFINED\n");

	return 0;
}

/*2015*///END======================================================

/*2017*///BEGIN======================================================

int command_set_kheap_plac_CONTALLOC(int number_of_arguments, char **arguments)
{
	setKHeapPlacementStrategyCONTALLOC();
	cprintf("Kernel Heap placement strategy is now FIRST FIT\n");
	return 0;
}

int command_set_kheap_plac_FIRSTFIT(int number_of_arguments, char **arguments)
{
	setKHeapPlacementStrategyFIRSTFIT();
	cprintf("Kernel Heap placement strategy is now FIRST FIT\n");
	return 0;
}

int command_set_kheap_plac_BESTFIT(int number_of_arguments, char **arguments)
{
	setKHeapPlacementStrategyBESTFIT();
	cprintf("Kernel Heap placement strategy is now BEST FIT\n");
	return 0;
}

int command_set_kheap_plac_NEXTFIT(int number_of_arguments, char **arguments)
{
	setKHeapPlacementStrategyNEXTFIT();
	cprintf("Kernel Heap placement strategy is now NEXT FIT\n");
	return 0;
}
int command_set_kheap_plac_WORSTFIT(int number_of_arguments, char **arguments)
{
	setKHeapPlacementStrategyWORSTFIT();
	cprintf("Kernel Heap placement strategy is now WORST FIT\n");
	return 0;
}

int command_print_kheap_plac(int number_of_arguments, char **arguments)
{
	if (isKHeapPlacementStrategyCONTALLOC())
		cprintf("Kernel Heap placement strategy is CONTINUOUS ALLOCATION\n");
	else if (isKHeapPlacementStrategyFIRSTFIT())
		cprintf("Kernel Heap placement strategy is FIRST FIT\n");
	else if (isKHeapPlacementStrategyBESTFIT())
		cprintf("Kernel Heap placement strategy is BEST FIT\n");
	else if (isKHeapPlacementStrategyNEXTFIT())
		cprintf("Kernel Heap placement strategy is NEXT FIT\n");
	else if (isKHeapPlacementStrategyWORSTFIT())
		cprintf("Kernel Heap placement strategy is WORST FIT\n");
	else
		cprintf("Kernel Heap placement strategy is UNDEFINED\n");

	return 0;
}

/*2017*///END======================================================

int command_disable_modified_buffer(int number_of_arguments, char **arguments)
{
	if(!isBufferingEnabled())
	{
		cprintf("Buffering is not enabled. Please enable buffering first.\n");
	}
	else
	{
		enableModifiedBuffer(0);
		cprintf("Modified Buffer is now DISABLED\n");
	}
	return 0;
}


int command_enable_modified_buffer(int number_of_arguments, char **arguments)
{
	if(!isBufferingEnabled())
	{
		cprintf("Buffering is not enabled. Please enable buffering first.\n");
	}
	else
	{
		enableModifiedBuffer(1);
		cprintf("Modified Buffer is now ENABLED\n");
	}
	return 0;
}

/*2016 ============================================================================*/

int command_disable_buffering(int number_of_arguments, char **arguments)
{
	enableBuffering(0);
	enableModifiedBuffer(0);
	cprintf("Buffering is now DISABLED\n");
	return 0;
}


int command_enable_buffering(int number_of_arguments, char **arguments)
{
	enableBuffering(1);
	enableModifiedBuffer(1);
	if(getModifiedBufferLength() == 0)
	{
		cprintf("Modified buffer enabled but with length = 0\n");
		char str[100];
		readline("Please enter the modified buff length = ", str);
		setModifiedBufferLength(strtol(str, NULL, 10));
		cprintf("Modified buffer length updated = %d\n", getModifiedBufferLength());
	}
	cprintf("Buffering is now ENABLED\n");
	return 0;
}

int command_set_modified_buffer_length(int number_of_arguments, char **arguments)
{
	if(!isBufferingEnabled())
	{
		cprintf("Buffering is not enabled. Please enable buffering to use the modified buffer.\n");
	}
	else if (!isModifiedBufferEnabled())
	{
		cprintf("Modified Buffering is not enabled. Please enable modified buffering.\n");
	}
	setModifiedBufferLength(strtol(arguments[1], NULL, 10));
	cprintf("Modified buffer length updated = %d\n", getModifiedBufferLength());
	return 0;
}

int command_get_modified_buffer_length(int number_of_arguments, char **arguments)
{
	if(!isBufferingEnabled())
	{
		cprintf("Buffering is not enabled. Please enable buffering to use the modified buffer.\n");
	}
	else if (!isModifiedBufferEnabled())
	{
		cprintf("Modified Buffering is not enabled. Please enable modified buffering.\n");
	}
	cprintf("Modified buffer length = %d\n", getModifiedBufferLength());
	return 0;
}

/*TESTING Commands*/
int command_test_kmalloc(int number_of_arguments, char **arguments)
{
	int testNum = 0 ;
	if (number_of_arguments==2)
		testNum = strtol(arguments[1], NULL, 10);
	if (isKHeapPlacementStrategyCONTALLOC())
		test_kmalloc();
	else if (isKHeapPlacementStrategyFIRSTFIT())
	{
		if (testNum == 0)
		{
			cprintf("Error: [Kernel.NextFit] must specify the test number (1 or 2) as an argument\n");
			return 0;
		}
		//Test FIRST FIT allocation
		if (testNum == 1)
			test_kmalloc_firstfit1();
		else if (testNum == 2)
			test_kmalloc_firstfit2();
	}
	else if (isKHeapPlacementStrategyBESTFIT())
	{
		if (testNum == 0)
		{
			cprintf("Error: [Kernel.BestFit] must specify the test number (1 or 2) as an argument\n");
			return 0;
		}
		if (testNum == 1)
			test_kmalloc();
		else if (testNum == 2)
			test_kmalloc_bestfit1();
		else if (testNum == 3)
			test_kmalloc_bestfit2();
	}
	else if (isKHeapPlacementStrategyNEXTFIT())
	{
		if (testNum == 0)
		{
			cprintf("Error: [Kernel.NextFit] must specify the test number (1 or 2) as an argument\n");
			return 0;
		}
		//Test cont. allocation
		if (testNum == 1)
			test_kmalloc();
		//Test nextfit strategy
		else if (testNum == 2)
			test_kmalloc_nextfit();

	}
	else if (isKHeapPlacementStrategyWORSTFIT())
		test_kmalloc_worstfit();
	return 0;
}
int command_test_kfree(int number_of_arguments, char **arguments)
{
	if (isKHeapPlacementStrategyBESTFIT())
	{
		test_kfree_bestfit();
	}
	else
	{
		test_kfree();
	}
	return 0;
}
int command_test_kheap_phys_addr(int number_of_arguments, char **arguments)
{
	test_kheap_phys_addr();
	return 0;
}
int command_test_kheap_virt_addr(int number_of_arguments, char **arguments)
{
	test_kheap_virt_addr();
	return 0;
}
int command_test_three_creation_functions(int number_of_arguments, char **arguments)
{
	test_three_creation_functions();
	return 0;
}

int command_test_krealloc(int number_of_arguments, char **arguments)
{
	return 0;
}

int command_test_priority1(int number_of_arguments, char **arguments)
{
	return 0;
}

int command_test_priority2(int number_of_arguments, char **arguments)
{
	return 0;
}

int command_test_kfreeall(int number_of_arguments, char **arguments)
{
	return 0;
}

int command_test_kexpand(int number_of_arguments, char **arguments)
{
	return 0;
}

int command_test_kshrink(int number_of_arguments, char **arguments)
{
	return 0;
}

int command_test_kfreelast(int number_of_arguments, char **arguments)
{
	return 0;
}

//END======================================================
