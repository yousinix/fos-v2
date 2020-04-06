/* See COPYRIGHT for copyright information. */

#ifndef FOS_KERN_ENV_H
#define FOS_KERN_ENV_H

#ifndef FOS_MULTIENV
// Change this value to 1 once you're allowing multiple environments
// (for UCLA: Lab 3, Part 3; for MIT: Lab 4).
#define FOS_MULTIENV 0
#endif

#include <inc/environment_definitions.h>

#define DECLARE_START_OF(binary_name)  \
	extern uint8 _binary_obj_user_##binary_name##_start[];

#define PTR_START_OF(binary_name) ( \
	(uint8*) _binary_obj_user_##binary_name##_start \
	)

//=========================================================
struct UserProgramInfo {
	const char *name;
	const char *desc;
	uint8* ptr_start;
};

//========================================================
//extern struct UserProgramInfo userPrograms[];
extern struct UserProgramInfo* ptr_UserPrograms;
extern int NUM_USER_PROGS;

//=========================================================

extern struct Env *envs;		// All environments
extern struct Env *curenv;	        // Current environment

///===================================================================================

struct UserProgramInfo*  get_user_program_info(char* user_program_name);
struct UserProgramInfo* get_user_program_info_by_env(struct Env* e);
//2016
struct Env* env_create(char* user_program_name, unsigned int page_WS_size, unsigned int percent_WS_pages_to_remove);
void	start_env_free(struct Env *e);

//2015
void env_exit();


// working set functions

///===================================================================================


void	env_destroy(struct Env *e);	// Does not return if e == curenv
void	env_init(void);
//int	env_alloc(struct Env **e, int32  parent_id);
// The following two functions do not return
void	env_run(struct Env *e) __attribute__((noreturn));
void	env_pop_tf(struct Trapframe *tf) __attribute__((noreturn));
void env_run_cmd_prmpt();

#endif // !FOS_KERN_ENV_H
