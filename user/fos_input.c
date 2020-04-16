#include <inc/lib.h>

void
_main(void)
{	
	int i1=0;
	int i2=0;
	char buff1[512];
	char buff2[512];


	atomic_readline("Please enter first number :", buff1);
	i1 = strtol(buff1, NULL, 10);

	//sleep
	env_sleep(2800);

	atomic_readline("Please enter second number :", buff2);
	
	i2 = strtol(buff2, NULL, 10);

	atomic_cprintf("number 1 + number 2 = %d\n",i1+i2);
	return;	
}
