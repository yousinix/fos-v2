#ifndef FOS_INC_SYSCALL_H
#define FOS_INC_SYSCALL_H

/* system call numbers */
enum
{
	SYS_cputs = 0,
	SYS_cgetc,
	SYS_getenvid,
	SYS_env_destroy,
	SYS_env_sleep,
	SYS_allocate_page,
	SYS_get_page,
	SYS_map_frame,
	SYS_unmap_frame,
	SYS_calc_req_frames,
	SYS_calc_free_frames,
	SYS_freeMem,
	NSYSCALLS
};

#endif /* !FOS_INC_SYSCALL_H */
