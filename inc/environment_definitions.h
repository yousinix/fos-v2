/* See COPYRIGHT for copyright information. */

#ifndef FOS_INC_ENV_H
#define FOS_INC_ENV_H

#include <inc/types.h>
#include <inc/queue.h>
#include <inc/trap.h>
#include <inc/memlayout.h>

// An environment ID 'envid_t' has three parts:
//
// +1+---------------21-----------------+--------10--------+
// |0|          Uniqueifier             |   Environment    |
// | |                                  |      Index       |
// +------------------------------------+------------------+
//                                       \--- ENVX(eid) --/
//
// The environment index ENVX(eid) equals the environment's offset in the
// 'envs[]' array.  The uniqueifier distinguishes environments that were
// created at different times, but share the same environment index.
//
// All real environments are greater than 0 (so the sign bit is zero).
// envid_ts less than 0 signify errors.

#define LOG2NENV		10
#define NENV			(1 << LOG2NENV)
#define ENVX(envid)		((envid) & (NENV - 1))

// Values of env_status in struct Env
#define ENV_FREE		0
#define ENV_RUNNABLE		1
#define ENV_NOT_RUNNABLE	2

struct Env {
	struct Trapframe env_tf;	// Saved registers
	LIST_ENTRY(Env) prev_next_info;	// Free list link pointers
	int32 env_id;			// Unique environment identifier
	int32 env_parent_id;		// env_id of this env's parent
	unsigned env_status;		// Status of the environment
	uint32 env_runs;		// Number of times environment has run

	// Address space
	uint32 *env_pgdir;		// Kernel virtual address of page dir
	uint32 env_cr3;		// Physical address of page dir

};

#endif // !FOS_INC_ENV_H
