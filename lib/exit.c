
#include <inc/lib.h>

void
destroy(void)
{
	sys_env_destroy(0);
}

void
exit(void)
{
	sys_env_exit();
}
