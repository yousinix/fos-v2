/* See COPYRIGHT for copyright information. */
#include <inc/mmu.h>
#include <inc/x86.h>
#include <inc/assert.h>

#include <kern/memory_manager.h>
#include <kern/console.h>
#include <kern/command_prompt.h>
#include <kern/user_environment.h>
#include <kern/file_manager.h>
#include <kern/syscall.h>
#include <kern/sched.h>
#include <kern/kclock.h>
#include <kern/utilities.h>

void scarce_memory()
{
	uint32 total_size_tobe_allocated = ((100 - memory_scarce_threshold_percentage)*number_of_frames)/100;
//	cprintf("total_size_tobe_allocated %d\n", number_of_frames);
	if (((100 - memory_scarce_threshold_percentage)*number_of_frames) % 100 > 0)
		total_size_tobe_allocated++;

	uint32 size_of_already_allocated = number_of_frames - LIST_SIZE(&free_frame_list) ;
	uint32 size_tobe_allocated = total_size_tobe_allocated - size_of_already_allocated;
//	cprintf("size_of_already_allocated %d\n", size_of_already_allocated);
//	cprintf("size to be allocated %d\n", size_tobe_allocated);
	int i = 0 ;
	struct Frame_Info* ptr_tmp_FI ;
	for (; i <= size_tobe_allocated ; i++)
	{
		allocate_frame(&ptr_tmp_FI) ;
	}
}

uint32 calc_no_pages_tobe_removed_from_ready_exit_queues(uint32 WS_or_MEMORY_flag)
{
	uint32 no_of_pages_tobe_removed_from_ready = 0;
	uint32 no_of_pages_tobe_removed_from_exit = 0;
	uint32 no_of_pages_tobe_removed_from_curenv = 0;
	if(WS_or_MEMORY_flag == 1)	// THEN MEMORY SHALL BE FREED
	{
		for(int i = 0; i < num_of_ready_queues; i++)
		{
			struct Env * ptr_ready_env = NULL;
			LIST_FOREACH(ptr_ready_env, &(env_ready_queues[i]))
			{
				int num_of_pages_in_WS = env_page_ws_get_size(ptr_ready_env);
				int num_of_pages_to_be_removed = curenv->percentage_of_WS_pages_to_be_removed * num_of_pages_in_WS / 100;
				if ((curenv->percentage_of_WS_pages_to_be_removed * num_of_pages_in_WS) % 100 > 0)
					num_of_pages_to_be_removed++;
				no_of_pages_tobe_removed_from_ready += num_of_pages_to_be_removed;
			}
		}

		struct Env * ptr_exit_env = NULL;
		LIST_FOREACH(ptr_exit_env, &env_exit_queue)
		{
			no_of_pages_tobe_removed_from_exit += env_page_ws_get_size(ptr_exit_env);
		}

		if(curenv != NULL)
		{
			int num_of_pages_in_WS = env_page_ws_get_size(curenv);
			int num_of_pages_to_be_removed = curenv->percentage_of_WS_pages_to_be_removed * num_of_pages_in_WS / 100;
			if ((curenv->percentage_of_WS_pages_to_be_removed * num_of_pages_in_WS) % 100 > 0)
				num_of_pages_to_be_removed++;
			no_of_pages_tobe_removed_from_curenv = num_of_pages_to_be_removed;
		}
	}
	else	// THEN RAPID PROCESS SHALL BE FREED ONLY
	{
		int num_of_pages_in_WS = env_page_ws_get_size(curenv);
		int num_of_pages_to_be_removed = curenv->percentage_of_WS_pages_to_be_removed * num_of_pages_in_WS / 100;
		if ((curenv->percentage_of_WS_pages_to_be_removed * num_of_pages_in_WS) % 100 > 0)
			num_of_pages_to_be_removed++;
		no_of_pages_tobe_removed_from_curenv = num_of_pages_to_be_removed;
	}

	return no_of_pages_tobe_removed_from_curenv + no_of_pages_tobe_removed_from_ready + no_of_pages_tobe_removed_from_exit;
}


void schenv()
{
	__nl = 0;
	__ne = NULL;
	for (int i = 0; i < num_of_ready_queues; ++i)
	{
		if (queue_size(&(env_ready_queues[i])))
		{
			__ne = LIST_LAST(&(env_ready_queues[i]));
			__nl = i;
			break;
		}
	}
	if (curenv != NULL)
	{
		if (__ne != NULL)
		{
			if ((__pl + 1) < __nl)
			{
				__ne = curenv;
				__nl = __pl < num_of_ready_queues-1? __pl + 1 : __pl ;
			}
		}
		else
		{
			__ne = curenv;
			__nl = __pl < num_of_ready_queues-1? __pl + 1 : __pl ;
		}
	}
}

void chksch(uint8 onoff)
{
	__pe = NULL;
	__ne = NULL;
	__pl = 0 ;
	__nl = 0 ;
	__chkstatus = onoff;
}
void chk1()
{
	if (__chkstatus == 0)
		return ;
	__pe = curenv;
	__pl = __nl ;
	if (__pe == NULL)
	{
		__pl = 0;
	}
	//cprintf("chk1: current = %s @ level %d\n", __pe == NULL? "NULL" : __pe->prog_name, __pl);
	schenv();
}
void chk2(struct Env* __se)
{
	if (__chkstatus == 0)
		return ;

	//cprintf("chk2: next = %s @ level %d\n", __ne == NULL? "NULL" : __ne->prog_name, __nl);

	assert_endall(__se == __ne);
	//cprintf("%d - %d\n", kclock_read_cnt0_latch() , TIMER_DIV((1000/quantums[__nl])));

	if (__ne != NULL)
	{
		uint16 upper = TIMER_DIV((1000/quantums[__nl])) ;
		upper = upper % 2 == 1? upper+1 : upper ;
		uint16 lower = 90 * upper / 100 ;
		uint16 current = kclock_read_cnt0();
		//cprintf("current = %d, lower = %d, upper = %d\n", current, lower, upper);
		assert_endall(current > lower && current <= upper) ;

		for (int i = 0; i < num_of_ready_queues; ++i)
		{
			assert_endall(find_env_in_queue(&(env_ready_queues[i]), __ne->env_id) == NULL);
		}
	}
	if (__pe != NULL && __pe != __ne)
	{
		uint8 __tl = __pl == num_of_ready_queues-1 ? __pl : __pl + 1 ;
		assert_endall(find_env_in_queue(&(env_ready_queues[__tl]), __pe->env_id) != NULL);
		for (int i = 0; i < num_of_ready_queues; ++i)
		{
			if (i == __tl) continue;
			assert_endall(find_env_in_queue(&(env_ready_queues[i]), __pe->env_id) == NULL) ;
		}
	}
}
