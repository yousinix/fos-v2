#include <inc/lib.h>


void
cputchar(int ch)
{
	char c = ch;

	// Unlike standard Unix's putchar,
	// the cputchar function _always_ outputs to the system console.
	//sys_cputs(&c, 1);

	sys_cputc(c);
}


void
atomic_cputchar(int ch)
{
	sys_disable_interrupt();
	char c = ch;

	// Unlike standard Unix's putchar,
	// the cputchar function _always_ outputs to the system console.
	//sys_cputs(&c, 1);

	sys_cputc(c);
	sys_enable_interrupt();
}

int
getchar(void)
{

	//return sys_cgetc();
	int c=0;
	while(c == 0)
	{
		c = sys_cgetc();
	}
	return c;
}

int
atomic_getchar(void)
{
	sys_disable_interrupt();
	int c=0;
	while(c == 0)
	{
		c = sys_cgetc();
	}
	sys_enable_interrupt();
	return c;
}

int iscons(int fdnum)
{
	// used by readline
	return 1;
}
