/* See COPYRIGHT for copyright information. */
/*
KEY WORDS
==========
MACROS: 	STATIC_KERNEL_PHYSICAL_ADDRESS, STATIC_KERNEL_VIRTUAL_ADDRESS, PDX, PTX, CONSTRUCT_ENTRY, EXTRACT_ADDRESS, ROUNDUP, ROUNDDOWN, LIST_INIT, LIST_INSERT_HEAD, LIST_FIRST, LIST_REMOVE
CONSTANTS:	PAGE_SIZE, PERM_PRESENT, PERM_WRITEABLE, PERM_USER, KERNEL_STACK_TOP, KERNEL_STACK_SIZE, KERNEL_BASE, READ_ONLY_FRAMES_INFO, PHYS_IO_MEM, PHYS_EXTENDED_MEM, E_NO_MEM
VARIABLES:	ptr_free_mem, ptr_page_directory, phys_page_directory, bootstack, Frame_Info, frames_info, free_frame_list, references, prev_next_info, size_of_extended_mem, number_of_frames, ptr_frame_info ,create, perm, va
FUNCTIONS:	to_physical_address, get_frame_info, tlb_invalidate
=====================================================================================================================================================================================================
 */

#include <kern/memory_manager.h>
#include <kern/helpers.h>
#include <kern/kheap.h>

// Global descriptor table.
//
// The kernel and user segments are identical(except for the DPL).
// To load the SS register, the CPL must equal the DPL.  Thus,
// we must duplicate the segments for the user and the kernel.
//
struct Segdesc gdt[] =
{
		// 0x0 - unused (always faults -- for trapping NULL far pointers)
		SEG_NULL,

		// 0x8 - kernel code segment
		[GD_KT >> 3] = SEG(STA_X | STA_R, 0x0, 0xffffffff, 0),

		// 0x10 - kernel data segment
		[GD_KD >> 3] = SEG(STA_W, 0x0, 0xffffffff, 0),

		// 0x18 - user code segment
		[GD_UT >> 3] = SEG(STA_X | STA_R, 0x0, 0xffffffff, 3),

		// 0x20 - user data segment
		[GD_UD >> 3] = SEG(STA_W, 0x0, 0xffffffff, 3),

		// 0x28 - tss, initialized in idt_init()
		[GD_TSS >> 3] = SEG_NULL
};

struct Pseudodesc gdt_pd =
{
		sizeof(gdt) - 1, (unsigned long) gdt
};

int nvram_read(int r)
{
	return mc146818_read(r) | (mc146818_read(r + 1) << 8);
}

void detect_memory()
{
	uint32 maxpa;	// Maximum physical address
	uint32 size_of_base_mem;		// Amount of base memory (in bytes)
	uint32 size_of_extended_mem;		// Amount of extended memory (in bytes)

	// CMOS tells us how many kilobytes there are
	size_of_base_mem = ROUNDDOWN(nvram_read(NVRAM_BASELO)*1024, PAGE_SIZE);
	size_of_extended_mem = ROUNDDOWN(nvram_read(NVRAM_EXTLO)*1024, PAGE_SIZE);

	//2016
	//For physical memory larger than 16MB, we needed to read total memory size
	// from a different register of the MC chip, see here:
	// http://bochs.sourceforge.net/techspec/CMOS-reference.txt
	// "CMOS 34h - AMI -"
	uint32 size_of_other_mem = ROUNDDOWN(nvram_read(0x34)*1024*64, PAGE_SIZE);
	//cprintf("other mem = %dK\n", size_of_other_mem/1024);

	// Calculate the maximum physical address based on whether
	// or not there is any extended memory.  See comment in ../inc/mmu.h.
	//2016
	if(size_of_other_mem > 0)
	{
		maxpa = size_of_other_mem + 16*1024*1024;
		size_of_extended_mem = maxpa - PHYS_EXTENDED_MEM;
	}
	else
	{
		if (size_of_extended_mem)
			maxpa = PHYS_EXTENDED_MEM + size_of_extended_mem;
		else
			maxpa = size_of_extended_mem;
	}

	uint32 kernel_virtual_area = ((0xFFFFFFFF-KERNEL_BASE)+1);
	if(USE_KHEAP == 0 && maxpa > kernel_virtual_area)
	{
		cprintf("Error!: Physical memory = %dK larger than kernel virtual area (%dK)\n", maxpa/1024, kernel_virtual_area/1024);
		cprintf("Cannot use physical memory larger than kernel virtual area\nTo enable physical memory larger than virtual kernel area, set USE_KHEAP = 1 in FOS code");
		while(1);
	}
	number_of_frames = maxpa / PAGE_SIZE;

	cprintf("Physical memory: %dK available, ", (int)(maxpa/1024));
	cprintf("base = %dK, extended = %dK\n", (int)(size_of_base_mem/1024), (int)(size_of_extended_mem/1024));
}

// --------------------------------------------------------------
// Set up initial memory mappings and turn on MMU.
// --------------------------------------------------------------

void check_boot_pgdir();

//
// Checks that the kernel part of virtual address space
// has been setup roughly correctly(by initialize_kernel_VM()).
//
// This function doesn't test every corner case,
// in fact it doesn't test the permission bits at all,
// but it is a pretty good check.
//
uint32 check_va2pa(uint32 *ptr_page_directory, uint32 va);

void check_boot_pgdir()
{
	uint32 i, n;

	// check frames_info array
	//2016: READ_ONLY_FRAMES_INFO not valid any more since it can't fit in 4 MB space
//	n = ROUNDUP(number_of_frames*sizeof(struct Frame_Info), PAGE_SIZE);
//	for (i = 0; i < n; i += PAGE_SIZE)
//	{
//		//cprintf("i = %x, arg 1  = %x, arg 2 = %x \n",i, check_va2pa(ptr_page_directory, READ_ONLY_FRAMES_INFO + i), STATIC_KERNEL_PHYSICAL_ADDRESS(frames_info) + i);
//		assert(check_va2pa(ptr_page_directory, READ_ONLY_FRAMES_INFO + i) == STATIC_KERNEL_PHYSICAL_ADDRESS(frames_info) + i);
//	}

	//2016
	// check phys mem
	if(USE_KHEAP)
	{
		for (i = 0; KERNEL_BASE + i < (uint32)ptr_free_mem; i += PAGE_SIZE)
			assert(check_va2pa(ptr_page_directory, KERNEL_BASE + i) == i);
	}
	else
	{
		for (i = 0; KERNEL_BASE + i != 0; i += PAGE_SIZE)
			assert(check_va2pa(ptr_page_directory, KERNEL_BASE + i) == i);
	}

	// check kernel stack
	for (i = 0; i < KERNEL_STACK_SIZE; i += PAGE_SIZE)
		assert(check_va2pa(ptr_page_directory, KERNEL_STACK_TOP - KERNEL_STACK_SIZE + i) == STATIC_KERNEL_PHYSICAL_ADDRESS(ptr_stack_bottom) + i);

	// check for zero/non-zero in PDEs
	for (i = 0; i < NPDENTRIES; i++) {
		switch (i) {
		case PDX(VPT):
		case PDX(UVPT):
		case PDX(KERNEL_STACK_TOP-1):
		case PDX(UENVS):
		//2016: READ_ONLY_FRAMES_INFO not valid any more since it can't fit in 4 MB space
		//case PDX(READ_ONLY_FRAMES_INFO):
		assert(ptr_page_directory[i]);
		break;
		default:
			if (i >= PDX(KERNEL_BASE))
				assert(ptr_page_directory[i]);
			else
				assert(ptr_page_directory[i] == 0);
			break;
		}
	}
	cprintf("check_boot_pgdir() succeeded!\n");
}

// This function returns the physical address of the page containing 'va',
// defined by the page directory 'ptr_page_directory'.  The hardware normally performs
// this functionality for us!  We define our own version to help check
// the check_boot_pgdir() function; it shouldn't be used elsewhere.

uint32 check_va2pa(uint32 *ptr_page_directory, uint32 va)
{
	uint32 *p;

	uint32* dirEntry = &(ptr_page_directory[PDX(va)]);

	//LOG_VARS("dir table entry %x", *dirEntry);

	if (!(*dirEntry & PERM_PRESENT))
		return ~0;
	p = (uint32*) STATIC_KERNEL_VIRTUAL_ADDRESS(EXTRACT_ADDRESS(*dirEntry));

	//LOG_VARS("ptr to page table  = %x", p);

	if (!(p[PTX(va)] & PERM_PRESENT))
		return ~0;

	//LOG_VARS("page phys addres = %x",EXTRACT_ADDRESS(p[PTX(va)]));
	return EXTRACT_ADDRESS(p[PTX(va)]);
}

void tlb_invalidate(uint32 *ptr_page_directory, void *virtual_address)
{
	// Flush the entry only if we're modifying the current address space.
	// For now, there is only one address space, so always invalidate.
	invlpg(virtual_address);
}

/*
void page_check()
{
	struct Frame_Info *pp, *pp0, *pp1, *pp2;
	struct Linked_List fl;

	// should be able to allocate three frames_info
	pp0 = pp1 = pp2 = 0;
	assert(allocate_frame(&pp0) == 0);
	assert(allocate_frame(&pp1) == 0);
	assert(allocate_frame(&pp2) == 0);

	assert(pp0);
	assert(pp1 && pp1 != pp0);
	assert(pp2 && pp2 != pp1 && pp2 != pp0);

	// temporarily steal the rest of the free frames_info
	fl = free_frame_list;
	LIST_INIT(&free_frame_list);

	// should be no free memory
	assert(allocate_frame(&pp) == E_NO_MEM);

	// there is no free memory, so we can't allocate a page table
	assert(map_frame(ptr_page_directory, pp1, 0x0, 0) < 0);

	// free pp0 and try again: pp0 should be used for page table
	free_frame(pp0);
	assert(map_frame(ptr_page_directory, pp1, 0x0, 0) == 0);
	assert(EXTRACT_ADDRESS(ptr_page_directory[0]) == to_physical_address(pp0));
	assert(check_va2pa(ptr_page_directory, 0x0) == to_physical_address(pp1));
	assert(pp1->references == 1);
	assert(pp0->references == 1);

	// should be able to map pp2 at PAGE_SIZE because pp0 is already allocated for page table
	assert(map_frame(ptr_page_directory, pp2, (void*) PAGE_SIZE, 0) == 0);
	assert(check_va2pa(ptr_page_directory, PAGE_SIZE) == to_physical_address(pp2));
	assert(pp2->references == 1);

	// should be no free memory
	assert(allocate_frame(&pp) == E_NO_MEM);

	// should be able to map pp2 at PAGE_SIZE because it's already there
	assert(map_frame(ptr_page_directory, pp2, (void*) PAGE_SIZE, 0) == 0);
	assert(check_va2pa(ptr_page_directory, PAGE_SIZE) == to_physical_address(pp2));
	assert(pp2->references == 1);

	// pp2 should NOT be on the free list
	// could happen in ref counts are handled sloppily in map_frame
	assert(allocate_frame(&pp) == E_NO_MEM);

	// should not be able to map at PTSIZE because need free frame for page table
	assert(map_frame(ptr_page_directory, pp0, (void*) PTSIZE, 0) < 0);

	// insert pp1 at PAGE_SIZE (replacing pp2)
	assert(map_frame(ptr_page_directory, pp1, (void*) PAGE_SIZE, 0) == 0);

	// should have pp1 at both 0 and PAGE_SIZE, pp2 nowhere, ...
	assert(check_va2pa(ptr_page_directory, 0) == to_physical_address(pp1));
	assert(check_va2pa(ptr_page_directory, PAGE_SIZE) == to_physical_address(pp1));
	// ... and ref counts should reflect this
	assert(pp1->references == 2);
	assert(pp2->references == 0);

	// pp2 should be returned by allocate_frame
	assert(allocate_frame(&pp) == 0 && pp == pp2);

	// unmapping pp1 at 0 should keep pp1 at PAGE_SIZE
	unmap_frame(ptr_page_directory, 0x0);
	assert(check_va2pa(ptr_page_directory, 0x0) == ~0);
	assert(check_va2pa(ptr_page_directory, PAGE_SIZE) == to_physical_address(pp1));
	assert(pp1->references == 1);
	assert(pp2->references == 0);

	// unmapping pp1 at PAGE_SIZE should free it
	unmap_frame(ptr_page_directory, (void*) PAGE_SIZE);
	assert(check_va2pa(ptr_page_directory, 0x0) == ~0);
	assert(check_va2pa(ptr_page_directory, PAGE_SIZE) == ~0);
	assert(pp1->references == 0);
	assert(pp2->references == 0);

	// so it should be returned by allocate_frame
	assert(allocate_frame(&pp) == 0 && pp == pp1);

	// should be no free memory
	assert(allocate_frame(&pp) == E_NO_MEM);

	// forcibly take pp0 back
	assert(EXTRACT_ADDRESS(ptr_page_directory[0]) == to_physical_address(pp0));
	if(USE_KHEAP)
	{
		kfree((void*)kheap_virtual_address(EXTRACT_ADDRESS(ptr_page_directory[0])));
	}
	else
	{
		ptr_page_directory[0] = 0;
		assert(pp0->references == 1);
		pp0->references = 0;
		free_frame(pp0);
	}
	// give free list back
	free_frame_list = fl;

	// free the frames_info we took
	free_frame(pp1);
	free_frame(pp2);

	cprintf("page_check() succeeded!\n");
}
*/

void turn_on_paging()
{
	//////////////////////////////////////////////////////////////////////
	// On x86, segmentation maps a VA to a LA (linear addr) and
	// paging maps the LA to a PA.  I.e. VA => LA => PA.  If paging is
	// turned off the LA is used as the PA.  Note: there is no way to
	// turn off segmentation.  The closest thing is to set the base
	// address to 0, so the VA => LA mapping is the identity.

	// Current mapping: VA (KERNEL_BASE+x) => PA (x).
	//     (segmentation base = -KERNEL_BASE and paging is off)

	// From here on down we must maintain this VA (KERNEL_BASE + x) => PA (x)
	// mapping, even though we are turning on paging and reconfiguring
	// segmentation.

	// Map VA 0:4MB same as VA (KERNEL_BASE), i.e. to PA 0:4MB.
	// (Limits our kernel to <4MB)

	//2016
	//ptr_page_directory[0] = ptr_page_directory[PDX(KERNEL_BASE)];
	{
		int i = PDX(KERNEL_BASE);
		int j = 0;
		for(; i< PDX((uint32)ptr_free_mem); ++i, ++j)
		{
			ptr_page_directory[j] = ptr_page_directory[i];
		}
	}

	// Install page table.
	lcr3(phys_page_directory);

	// Turn on paging.
	uint32 cr0;
	cr0 = rcr0();
	cr0 |= CR0_PE|CR0_PG|CR0_AM|CR0_WP|CR0_NE|CR0_TS|CR0_EM|CR0_MP;
	cr0 &= ~(CR0_TS|CR0_EM);
	lcr0(cr0);

	// Current mapping: KERNEL_BASE+x => x => x.
	// (x < 4MB so uses paging ptr_page_directory[0])

	// Reload all segment registers.
	asm volatile("lgdt gdt_pd");
	asm volatile("movw %%ax,%%gs" :: "a" (GD_UD|3));
	asm volatile("movw %%ax,%%fs" :: "a" (GD_UD|3));
	asm volatile("movw %%ax,%%es" :: "a" (GD_KD));
	asm volatile("movw %%ax,%%ds" :: "a" (GD_KD));
	asm volatile("movw %%ax,%%ss" :: "a" (GD_KD));
	asm volatile("ljmp %0,$1f\n 1:\n" :: "i" (GD_KT));  // reload cs
	asm volatile("lldt %%ax" :: "a" (0));

	// Final mapping: KERNEL_BASE + x => KERNEL_BASE + x => x.

	// This mapping was only used after paging was turned on but
	// before the segment registers were reloaded.
	//2016
	//ptr_page_directory[0] = 0;
	{
		int i = PDX(KERNEL_BASE);
		int j = 0;
		for(; i< PDX((uint32)ptr_free_mem); ++i, ++j)
		{
			ptr_page_directory[j] = 0;
		}
	}
	// Flush the TLB for good measure, to kill the ptr_page_directory[0] mapping.
	lcr3(phys_page_directory);
}

void setup_listing_to_all_page_tables_entries()
{
	//////////////////////////////////////////////////////////////////////
	// Recursively insert PD in itself as a page table, to form
	// a virtual page table at virtual address VPT.

	// Permissions: kernel RW, user NONE
	uint32 phys_frame_address = STATIC_KERNEL_PHYSICAL_ADDRESS(ptr_page_directory);
	ptr_page_directory[PDX(VPT)] = CONSTRUCT_ENTRY(phys_frame_address , PERM_PRESENT | PERM_WRITEABLE);

	// same for UVPT
	//Permissions: kernel R, user R
	ptr_page_directory[PDX(UVPT)] = STATIC_KERNEL_PHYSICAL_ADDRESS(ptr_page_directory)|PERM_USER|PERM_PRESENT;

}

//
// Converts an envid to an env pointer.
//
// RETURNS
//   0 on success, -E_BAD_ENV on error.
//   On success, sets *penv to the environment.
//   On error, sets *penv to NULL.
//
int envid2env(int32  envid, struct Env **env_store, bool checkperm)
{
	struct Env *e;

	// If envid is zero, return the current environment.
	if (envid == 0) {
		*env_store = curenv;
		return 0;
	}

	// Look up the Env structure via the index part of the envid,
	// then check the env_id field in that struct Env
	// to ensure that the envid is not stale
	// (i.e., does not refer to a _previous_ environment
	// that used the same slot in the envs[] array).
	e = &envs[ENVX(envid)];
	if (e->env_status == ENV_FREE || e->env_id != envid) {
		*env_store = 0;
		return E_BAD_ENV;
	}

	// Check that the calling environment has legitimate permission
	// to manipulate the specified environment.
	// If checkperm is set, the specified environment
	// must be either the current environment
	// or an immediate child of the current environment.
	if (checkperm && e != curenv && e->env_parent_id != curenv->env_id) {
		*env_store = 0;
		return E_BAD_ENV;
	}

	*env_store = e;
	return 0;
}

//

