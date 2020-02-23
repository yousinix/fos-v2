#include <inc/lib.h>
	
void
_main(void)
{	
	int i=28;
	for(;i<128; i++)
	{
		int c=0;
		for(;c<10; c++)
		{
			cprintf("%c",i);
		}
		int d=0;
		for(; d< 500000; d++);	
		c=0;
		for(;c<10; c++)
		{
			cprintf("\b");
		}		
	}
	
	return;	
}
