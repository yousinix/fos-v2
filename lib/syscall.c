// System call stubs.

#include <inc/syscall.h>
#include <inc/lib.h>

static inline uint32
syscall(int num, uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5)
{
	uint32 ret;

	// Generic system call: pass system call number in AX,
	// up to five parameters in DX, CX, BX, DI, SI.
	// Interrupt kernel with T_SYSCALL.
	//
	// The "volatile" tells the assembler not to optimize
	// this instruction away just because we don't use the
	// return value.
	// 
	// The last clause tells the assembler that this can
	// potentially change the condition codes and arbitrary
	// memory locations.

	asm volatile("int %1\n"
		: "=a" (ret)
		: "i" (T_SYSCALL),
		  "a" (num),
		  "d" (a1),
		  "c" (a2),
		  "b" (a3),
		  "D" (a4),
		  "S" (a5)
		: "cc", "memory");
	
	return ret;
}

void
sys_cputs(const char *s, uint32 len)
{
	syscall(SYS_cputs, (uint32) s, len, 0, 0, 0);
}

int
sys_cgetc(void)
{
	return syscall(SYS_cgetc, 0, 0, 0, 0, 0);
}

int	sys_env_destroy(int32  envid)
{
	return syscall(SYS_env_destroy, envid, 0, 0, 0, 0);
}

int32 sys_getenvid(void)
{
	 return syscall(SYS_getenvid, 0, 0, 0, 0, 0);
}

void sys_env_sleep(void)
{
	syscall(SYS_env_sleep, 0, 0, 0, 0, 0);
}


int sys_allocate_page(void *va, int perm)
{
	return syscall(SYS_allocate_page, (uint32) va, perm, 0 , 0, 0);
}

int sys_get_page(void *va, int perm)
{
	return syscall(SYS_get_page, (uint32) va, perm, 0 , 0, 0);
}
		
int sys_map_frame(int32 srcenv, void *srcva, int32 dstenv, void *dstva, int perm)
{
	return syscall(SYS_map_frame, srcenv, (uint32) srcva, dstenv, (uint32) dstva, perm);
}

int sys_unmap_frame(int32 envid, void *va)
{
	return syscall(SYS_unmap_frame, envid, (uint32) va, 0, 0, 0);
}

uint32 sys_calculate_required_frames(uint32 start_virtual_address, uint32 size)
{
	return syscall(SYS_calc_req_frames, start_virtual_address, (uint32) size, 0, 0, 0);
}

uint32 sys_calculate_free_frames()
{
	return syscall(SYS_calc_free_frames, 0, 0, 0, 0, 0);
}

void sys_freeMem(void* start_virtual_address, uint32 size)
{
	syscall(SYS_freeMem, (uint32) start_virtual_address, size, 0, 0, 0);
	return;
}
