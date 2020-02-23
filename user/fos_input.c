
#include <inc/lib.h>

void
_main(void)
{	
	int i1=0;
	int i2=0;
	char buff1[256];
	char buff2[256];	
	
	readline("Please enter first number :", buff1);	
	i1 = strtol(buff1, NULL, 10);
	readline("Please enter second number :", buff2);
	
	i2 = strtol(buff2, NULL, 10);

	cprintf("number 1 + number 2 = %d\n",i1+i2);
	return;	
}
