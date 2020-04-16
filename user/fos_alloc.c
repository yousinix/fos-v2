#include <inc/lib.h>


void _main(void)
{	
	//uint32 size = 2*1024*1024 +120*4096+1;
	//uint32 size = 1*1024*1024 + 256*1024;
	//uint32 size = 1*1024*1024;
	uint32 size = 100;

	unsigned char *x = malloc(sizeof(unsigned char)*size) ;
	atomic_cprintf("x allocated at %x\n",x);

	//unsigned char *y = malloc(sizeof(unsigned char)*size) ;
	//cprintf("y allocated at %x\n",y);

	//unsigned char *z = malloc(sizeof(unsigned char)*size) ;
	//cprintf("z allocated at %x\n",z);
	
	int i ;
	for (i = 0 ; i < size ; i++)
	{
		x[i] = i%256 ;
		////y[i] = 10 ;
		////y[i] = i%256 ;
		////z[i] = (int)(x[i]  * y[i]);
		////z[i] = i%256;
	}

	
	for (i = size-7 ; i < size ; i++)
		atomic_cprintf("x[%d] = %d\n",i, x[i]);
	
	free(x);

	x = malloc(sizeof(unsigned char)*size) ;
	
	for (i = size-7 ; i < size ; i++)
	{
		atomic_cprintf("x[%d] = %d\n",i,x[i]);
	}

	free(x);
	
	return;	
}

