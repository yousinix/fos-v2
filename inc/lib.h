// Main public header file for our user-land support library,
// whose code lives in the lib directory.
// This library is roughly our OS's version of a standard C library,
// and is intended to be linked into all user-mode applications
// (NOT the kernel or boot loader).

#ifndef FOS_INC_LIB_H
#define FOS_INC_LIB_H 1

#include <inc/types.h>
#include <inc/stdio.h>
#include <inc/stdarg.h>
#include <inc/string.h>
#include <inc/error.h>
#include <inc/assert.h>
#include <inc/environment_definitions.h>
#include <inc/memlayout.h>
#include <inc/syscall.h>
#include <inc/malloc.h>

#define USED(x)		(void)(x)

// libos.c or entry.S
extern char *binaryname;
extern volatile struct Env *env;
extern volatile struct Env envs[NENV];
extern volatile struct Frame_Info frames_info[];
void	exit(void);
void	sleep(void);

// readline.c
void readline(const char *buf, char*);

// syscall.c
void	sys_cputs(const char *string, uint32 len);
int	sys_cgetc(void);
int32	sys_getenvid(void);
int	sys_env_destroy(int32);
void 	sys_env_sleep();
int 	sys_allocate_page(void *va, int perm);
int 	sys_get_page(void *va, int perm);
int 	sys_map_frame(int32 srcenv, void *srcva, int32 dstenv, void *dstva, int perm);
int 	sys_unmap_frame(int32 envid, void *va);
uint32 	sys_calculate_required_frames(uint32 start_virtual_address, uint32 size);
uint32 	sys_calculate_free_frames();
void 	sys_freeMem(void* start_virtual_address, uint32 size) ;

// console.c
void	cputchar(int c);
int	getchar(void);
int	iscons(int fd);
int	opencons(void);

/* File open modes */
#define	O_RDONLY	0x0000		/* open for reading only */
#define	O_WRONLY	0x0001		/* open for writing only */
#define	O_RDWR		0x0002		/* open for reading and writing */
#define	O_ACCMODE	0x0003		/* mask for above modes */

#define	O_CREAT		0x0100		/* create if nonexistent */
#define	O_TRUNC		0x0200		/* truncate to zero length */
#define	O_EXCL		0x0400		/* error if already exists */
#define O_MKDIR		0x0800		/* create directory, not regular file */

#endif	// !FOS_INC_LIB_H
