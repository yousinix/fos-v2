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
sys_cputs(const char *s, uint32 len, uint8 printProgName)
{
	syscall(SYS_cputs, (uint32) s, len, (uint32)printProgName, 0, 0);
}

int
sys_cgetc(void)
{
	return syscall(SYS_cgetc, 0, 0, 0, 0, 0);
}

int sys_env_destroy(int32  envid)
{
	return syscall(SYS_env_destroy, envid, 0, 0, 0, 0);
}

int32 sys_getenvid(void)
{
	 return syscall(SYS_getenvid, 0, 0, 0, 0, 0);
}

//2017
int32 sys_getenvindex(void)
{
	 return syscall(SYS_getenvindex, 0, 0, 0, 0, 0);
}

int32 sys_getparentenvid(void)
{
	 return syscall(SYS_getparentenvid, 0, 0, 0, 0, 0);
}


void sys_env_exit(void)
{
	syscall(SYS_env_exit, 0, 0, 0, 0, 0);
}


int __sys_allocate_page(void *va, int perm)
{
	return syscall(SYS_allocate_page, (uint32) va, perm, 0 , 0, 0);
}

int __sys_map_frame(int32 srcenv, void *srcva, int32 dstenv, void *dstva, int perm)
{
	return syscall(SYS_map_frame, srcenv, (uint32) srcva, dstenv, (uint32) dstva, perm);
}

int __sys_unmap_frame(int32 envid, void *va)
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
uint32 sys_calculate_modified_frames()
{
	return syscall(SYS_calc_modified_frames, 0, 0, 0, 0, 0);
}

uint32 sys_calculate_notmod_frames()
{
	return syscall(SYS_calc_notmod_frames, 0, 0, 0, 0, 0);
}

void sys_freeMem(uint32 virtual_address, uint32 size)
{
	syscall(SYS_freeMem, virtual_address, size, 0, 0, 0);
	return;
}

void sys_allocateMem(uint32 virtual_address, uint32 size)
{
	syscall(SYS_allocateMem, virtual_address, size, 0, 0, 0);
	return ;
}

int sys_pf_calculate_allocated_pages()
{
	return syscall(SYS_pf_calc_allocated_pages, 0,0,0,0,0);
}

int sys_calculate_pages_tobe_removed_ready_exit(uint32 WS_or_MEMORY_flag)
{
	return syscall(SYS_calculate_pages_tobe_removed_ready_exit, WS_or_MEMORY_flag,0,0,0,0);
}

void sys_scarce_memory()
{
	syscall(SYS_scarce_memory,0,0,0,0,0);
}

//NEW !! 2012...
void
sys_disable_interrupt()
{
	syscall(SYS_disableINTR,0, 0, 0, 0, 0);
}


void
sys_enable_interrupt()
{
	syscall(SYS_enableINTR,0, 0, 0, 0, 0);
}


void
sys_cputc(const char c)
{
	syscall(SYS_cputc, (uint32) c, 0, 0, 0, 0);
}


//NEW'12: BONUS2 Testing
void
sys_clear_ffl()
{
	syscall(SYS_clearFFL,0, 0, 0, 0, 0);
}

int
sys_createSemaphore(char* semaphoreName, uint32 initialValue)
{
	return syscall(SYS_create_semaphore,(uint32)semaphoreName, (uint32)initialValue, 0, 0, 0);
}

int
sys_getSemaphoreValue(int32 ownerEnvID, char* semaphoreName)
{
	return syscall(SYS_get_semaphore_value,(uint32) ownerEnvID, (uint32)semaphoreName, 0, 0, 0);
}

void
sys_waitSemaphore(int32 ownerEnvID, char* semaphoreName)
{
	syscall(SYS_wait_semaphore,(uint32) ownerEnvID, (uint32)semaphoreName, 0, 0, 0);
}

void
sys_signalSemaphore(int32 ownerEnvID, char* semaphoreName)
{
	syscall(SYS_signal_semaphore,(uint32) ownerEnvID, (uint32)semaphoreName, 0, 0, 0);
}

int
sys_createSharedObject(char* shareName, uint32 size, uint8 isWritable, void* virtual_address)
{
	return syscall(SYS_create_shared_object,(uint32)shareName, (uint32)size, isWritable, (uint32)virtual_address,  0);
}

//2017:
int
sys_getSizeOfSharedObject(int32 ownerID, char* shareName)
{
	return syscall(SYS_get_size_of_shared_object,(uint32) ownerID, (uint32)shareName, 0, 0, 0);
}
//==========

int
sys_getSharedObject(int32 ownerID, char* shareName, void* virtual_address)
{
	return syscall(SYS_get_shared_object,(uint32) ownerID, (uint32)shareName, (uint32)virtual_address, 0, 0);
}

int
sys_freeSharedObject(int32 sharedObjectID, void *startVA)
{
	return syscall(SYS_free_shared_object,(uint32) sharedObjectID, (uint32) startVA, 0, 0, 0);
}

uint32 	sys_getMaxShares()
{
	return syscall(SYS_get_max_shares,0, 0, 0, 0, 0);
}

int
sys_create_env(char* programName, unsigned int page_WS_size, unsigned int percent_WS_pages_to_remove)
{
	return syscall(SYS_create_env,(uint32)programName, page_WS_size, percent_WS_pages_to_remove, 0, 0);
}

void
sys_run_env(int32 envId)
{
	syscall(SYS_run_env, (int32)envId, 0, 0, 0, 0);
}

void
sys_free_env(int32 envId)
{
	syscall(SYS_free_env, (int32)envId, 0, 0, 0, 0);
}

struct uint64
sys_get_virtual_time()
{
	struct uint64 result;
	syscall(SYS_get_virtual_time, (uint32)&(result.low), (uint32)&(result.hi), 0, 0, 0);
	return result;
}

// 2014
void sys_moveMem(uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
{
	syscall(SYS_moveMem, src_virtual_address, dst_virtual_address, size, 0, 0);
	return ;
}
uint32 sys_rcr2()
{
	return syscall(SYS_rcr2, 0, 0, 0, 0, 0);
}
void sys_bypassPageFault(uint8 instrLength)
{
	syscall(SYS_bypassPageFault, instrLength, 0, 0, 0, 0);
	return ;
}
void rsttst()
{
	syscall(SYS_rsttst, 0, 0, 0, 0, 0);
	return ;
}
void tst(uint32 n, uint32 v1, uint32 v2, char c, int inv)
{
	syscall(SYS_testNum, n, v1, v2, c, inv);
	return ;
}
void chktst(uint32 n)
{
	syscall(SYS_chktst, n, 0, 0, 0, 0);
	return ;
}

void inctst()
{
	syscall(SYS_inctst, 0, 0, 0, 0, 0);
	return ;
}
uint32 gettst()
{
	return syscall(SYS_gettst, 0, 0, 0, 0, 0);
}


//2015
uint32 sys_isUHeapPlacementStrategyFIRSTFIT()
{
	uint32 ret = syscall(SYS_get_heap_strategy, 0, 0, 0, 0, 0);
	if (ret == UHP_PLACE_FIRSTFIT)
		return 1;
	else
		return 0;
}
uint32 sys_isUHeapPlacementStrategyBESTFIT()
{
	uint32 ret = syscall(SYS_get_heap_strategy, 0, 0, 0, 0, 0);
	if (ret == UHP_PLACE_BESTFIT)
		return 1;
	else
		return 0;
}
uint32 sys_isUHeapPlacementStrategyNEXTFIT()
{
	uint32 ret = syscall(SYS_get_heap_strategy, 0, 0, 0, 0, 0);
	if (ret == UHP_PLACE_NEXTFIT)
		return 1;
	else
		return 0;
}
uint32 sys_isUHeapPlacementStrategyWORSTFIT()
{
	uint32 ret = syscall(SYS_get_heap_strategy, 0, 0, 0, 0, 0);
	if (ret == UHP_PLACE_WORSTFIT)
		return 1;
	else
		return 0;
}

void sys_set_uheap_strategy(uint32 heapStrategy)
{
	syscall(SYS_set_heap_strategy, heapStrategy, 0, 0, 0, 0);
	return ;
}

