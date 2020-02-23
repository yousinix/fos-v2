/* See COPYRIGHT for copyright information. */

#include <inc/x86.h>
#include <inc/error.h>
#include <inc/string.h>
#include <inc/assert.h>

#include <kern/user_environment.h>
#include <kern/memory_manager.h>
#include <kern/trap.h>
#include <kern/syscall.h>
#include <kern/console.h>

// Print a string to the system console.
// The string is exactly 'len' characters long.
// Destroys the environment on memory errors.
static void sys_cputs(const char *s, uint32 len)
{
	// Check that the user has permission to read memory [s, s+len).
	// Destroy the environment if not.
	
	// LAB 3: Your code here.

	// Print the string supplied by the user.
	cprintf("%.*s", len, s);
}

// Read a character from the system console.
// Returns the character.
static int
sys_cgetc(void)
{
	int c;

	// The cons_getc() primitive doesn't wait for a character,
	// but the sys_cgetc() system call does.
	while ((c = cons_getc()) == 0)
		/* do nothing */;

	return c;
}

// Returns the current environment's envid.
static int32 sys_getenvid(void)
{
	return curenv->env_id;
}

// Destroy a given environment (possibly the currently running environment).
//
// Returns 0 on success, < 0 on error.  Errors are:
//	-E_BAD_ENV if environment envid doesn't currently exist,
//		or the caller doesn't have permission to change envid.
static int sys_env_destroy(int32  envid)
{
	int r;
	struct Env *e;

	if ((r = envid2env(envid, &e, 1)) < 0)
		return r;
	if (e == curenv)
		cprintf("[%08x] exiting gracefully\n", curenv->env_id);
	else
		cprintf("[%08x] destroying %08x\n", curenv->env_id, e->env_id);
	env_destroy(e);
	return 0;
}

static void sys_env_sleep()
{
	env_run_cmd_prmpt();
}
	

// Allocate a page of memory and map it at 'va' with permission
// 'perm' in the address space of 'envid'.
// The page's contents are set to 0.
// If a page is already mapped at 'va', that page is unmapped as a
// side effect.
//
// perm -- PTE_U | PTE_P must be set, PTE_AVAIL | PTE_W may or may not be set,
//         but no other bits may be set.
//
// Return 0 on success, < 0 on error.  Errors are:
//	E_BAD_ENV if environment envid doesn't currently exist,
//		or the caller doesn't have permission to change envid.
//	E_INVAL if va >= UTOP, or va is not page-aligned.
//	E_INVAL if perm is inappropriate (see above).
//	E_NO_MEM if there's no memory to allocate the new page,
//		or to allocate any necessary page tables.
static int sys_allocate_page(void *va, int perm)
{
	// Hint: This function is a wrapper around page_alloc() and
	//   page_insert() from kern/pmap.c.
	//   Most of the new code you write should be to check the
	//   parameters for correctness.
	//   If page_insert() fails, remember to free the page you
	//   allocated!
	
	int r;
	struct Env *e = curenv;

	//if ((r = envid2env(envid, &e, 1)) < 0)
		//return r;
	
	struct Frame_Info *ptr_frame_info ;
	r = allocate_frame(&ptr_frame_info) ;
	if (r == E_NO_MEM)
		return r ;
	
	//check virtual address to be paged_aligned and < USER_TOP
	if ((uint32)va >= USER_TOP || (uint32)va % PAGE_SIZE != 0)
		return E_INVAL;
	
	//check permissions to be appropriatess
	if ((perm & (~PERM_AVAILABLE & ~PERM_WRITEABLE)) != (PERM_USER))
		return E_INVAL;
	
			
	uint32 physical_address = to_physical_address(ptr_frame_info) ;
	
	memset(K_VIRTUAL_ADDRESS(physical_address), 0, PAGE_SIZE);
		
	r = map_frame(e->env_pgdir, ptr_frame_info, va, perm) ;
	if (r == E_NO_MEM)
	{
		decrement_references(ptr_frame_info);
		return r;
	}
	return 0 ;
}

// Allocate a page of memory and map it at 'va' with permission
// 'perm' in the address space of 'envid'.
// The page's contents are set to 0.
// If a page is already mapped at 'va', that function does nothing
//
// Return 0 on success, < 0 on error.  Errors are:
//	E_BAD_ENV if environment envid doesn't currently exist,
//		or the caller doesn't have permission to change envid.
//	E_INVAL if va >= UTOP, or va is not page-aligned.
//	E_INVAL if perm is inappropriate (see above).
//	E_NO_MEM if there's no memory to allocate the new page,
//		or to allocate any necessary page tables.
static int sys_get_page(void *va, int perm)
{
	return get_page(curenv->env_pgdir, va, perm) ;
}

// Map the page of memory at 'srcva' in srcenvid's address space
// at 'dstva' in dstenvid's address space with permission 'perm'.
// Perm has the same restrictions as in sys_page_alloc, except
// that it also must not grant write access to a read-only
// page.
//
// Return 0 on success, < 0 on error.  Errors are:
//	-E_BAD_ENV if srcenvid and/or dstenvid doesn't currently exist,
//		or the caller doesn't have permission to change one of them.
//	-E_INVAL if srcva >= UTOP or srcva is not page-aligned,
//		or dstva >= UTOP or dstva is not page-aligned.
//	-E_INVAL is srcva is not mapped in srcenvid's address space.
//	-E_INVAL if perm is inappropriate (see sys_page_alloc).
//	-E_INVAL if (perm & PTE_W), but srcva is read-only in srcenvid's
//		address space.
//	-E_NO_MEM if there's no memory to allocate the new page,
//		or to allocate any necessary page tables.
static int sys_map_frame(int32 srcenvid, void *srcva, int32 dstenvid, void *dstva, int perm)
{
	// Hint: This function is a wrapper around page_lookup() and
	//   page_insert() from kern/pmap.c.
	//   Again, most of the new code you write should be to check the
	//   parameters for correctness.
	//   Use the third argument to page_lookup() to
	//   check the current permissions on the page.

	// LAB 4: Your code here.
	panic("sys_map_frame not implemented");
}

// Unmap the page of memory at 'va' in the address space of 'envid'.
// If no page is mapped, the function silently succeeds.
//
// Return 0 on success, < 0 on error.  Errors are:
//	-E_BAD_ENV if environment envid doesn't currently exist,
//		or the caller doesn't have permission to change envid.
//	-E_INVAL if va >= UTOP, or va is not page-aligned.
static int sys_unmap_frame(int32 envid, void *va)
{
	// Hint: This function is a wrapper around page_remove().
	
	// LAB 4: Your code here.
	panic("sys_page_unmap not implemented");
}

uint32 sys_calculate_required_frames(uint32 start_virtual_address, uint32 size)
{
	return calculate_required_frames(curenv->env_pgdir, start_virtual_address, size); 
}

uint32 sys_calculate_free_frames()
{
	return calculate_free_frames();
}
void sys_freeMem(void* start_virtual_address, uint32 size)
{
	freeMem((uint32*)curenv->env_pgdir, (void*)start_virtual_address, size);
	return;
}
// Dispatches to the correct kernel function, passing the arguments.
uint32
syscall(uint32 syscallno, uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5)
{
	// Call the function corresponding to the 'syscallno' parameter.
	// Return any appropriate return value.
	// LAB 3: Your code here.
	switch(syscallno)
	{
		case SYS_cputs:
			sys_cputs((const char*)a1,a2);
			return 0;
			break;
		case SYS_cgetc:
			return sys_cgetc();
			break;
		case SYS_getenvid:
			return sys_getenvid();
			break;
		case SYS_env_destroy:
			return sys_env_destroy(a1);
			break;
		case SYS_env_sleep:
			sys_env_sleep();
			return 0;
			break;
		case SYS_calc_req_frames:
			return sys_calculate_required_frames(a1, a2);			
			break;
		case SYS_calc_free_frames:
			return sys_calculate_free_frames();			
			break;
		case SYS_freeMem:
			sys_freeMem((void*)a1, a2);
			return 0;			
			break;
		//======================
		
		case SYS_allocate_page:
			sys_allocate_page((void*)a1, a2);
			return 0;
			break;
		case SYS_get_page:
			sys_get_page((void*)a1, a2);
			return 0;
		break;case SYS_map_frame:
			sys_map_frame(a1, (void*)a2, a3, (void*)a4, a5);
			return 0;
			break;
		case SYS_unmap_frame:
			sys_unmap_frame(a1, (void*)a2);
			return 0;
			break;
		case NSYSCALLS:	
			return 	-E_INVAL;
			break;
	}
	//panic("syscall not implemented");
	return -E_INVAL;
}

