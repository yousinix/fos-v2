
#include <inc/lib.h>

void
_main(void)
{	
	int size = 10 ;
	int *x = malloc(sizeof(int)*size) ;
	int *y = malloc(sizeof(int)*size) ;
	int *z = malloc(sizeof(int)*size) ;

	int i ;
	for (i = 0 ; i < size ; i++)
	{
		x[i] = i ;
		y[i] = 10 ;
		z[i] = (int)x[i]  * y[i]  ;
	}
	
	for (i = 0 ; i < size ; i++)
		cprintf("%d * %d = %d\n",x[i], y[i], z[i]);
	
	freeHeap();
	cprintf("the heap is freed successfully\n");
	z = malloc(sizeof(int)*size) ;
	for (i = 0 ; i < size ; i++)
	{
		cprintf("x[i] = %d\t",x[i]);
		cprintf("y[i] = %d\t",y[i]);
		cprintf("z[i] = %d\n",z[i]);
	
	}

	return;	
}
