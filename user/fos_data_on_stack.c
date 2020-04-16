#include <inc/lib.h>


void _main(void)
{
	/// Adding array of 512 integer on user stack
	int arr[2512];

	atomic_cprintf("user stack contains 512 integer\n");

	return;	
}
