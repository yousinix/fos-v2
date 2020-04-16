
#include <inc/lib.h>


int factorial(int n);

void
_main(void)
{
	int i1=0;
	char buff1[256];
	//atomic_readline("Please enter a number:", buff1);
	i1 = 10;

	int res = factorial(i1) ;

	atomic_cprintf("Factorial %d = %d\n",i1, res);

	return;
}


int factorial(int n)
{
	if (n <= 1)
		return 1 ;
	return n * factorial(n-1) ;
}

