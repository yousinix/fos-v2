#include <inc/lib.h>

/// Adding array of 20000 integer on user data section
int arr[20000];

void _main(void)
{	
	atomic_cprintf("user data section contains 20,000 integer\n");
	
	return;	
}
