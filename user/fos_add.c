// hello, world
#include <inc/lib.h>

void
_main(void)
{
	int i1=0;
	int i2=0;

	i1 = strtol("1", NULL, 10);
	i2 = strtol("2", NULL, 10);

	atomic_cprintf("number 1 + number 2 = %d\n",i1+i2);
	//cprintf("number 1 + number 2 = \n");
	return;
}
