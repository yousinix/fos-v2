// Called from entry.S to get us going.
// entry.S already took care of defining envs, pages, vpd, and vpt.

#include <inc/lib.h>

extern void _main(int argc, char **argv);

volatile struct Env *myEnv = NULL;
volatile char *binaryname = "(PROGRAM NAME UNKNOWN)";

void
libmain(int argc, char **argv)
{
	int envIndex = sys_getenvindex();
	myEnv = &(envs[envIndex]);

	//SET THE PROGRAM NAME
	if (myEnv->prog_name[0] != '\0')
		binaryname = myEnv->prog_name;

	// set env to point at our env structure in envs[].
	// env = envs;

	// save the name of the program so that panic() can use it
	if (argc > 0)
		binaryname = argv[0];

	// call user main routine
	_main(argc, argv);



	sys_disable_interrupt();
	cprintf("**************************************\n");
	cprintf("Num of PAGE faults = %d, modif = %d\n", myEnv->pageFaultsCounter, myEnv->nModifiedPages);
	//cprintf("Num of freeing scarce memory = %d, freeing full working set = %d\n", myEnv->freeingScarceMemCounter, myEnv->freeingFullWSCounter);
	cprintf("Num of clocks = %d\n", myEnv->nClocks);
	cprintf("**************************************\n");
	sys_enable_interrupt();

	// exit gracefully
	exit();
}

