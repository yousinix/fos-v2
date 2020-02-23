/* See COPYRIGHT for copyright information. */

#ifndef FOS_KERN_MEM_MAN_H
#define FOS_KERN_MEM_MAN_H
#ifndef FOS_KERNEL
# error "This is a FOS kernel header; user programs should not #include it"
#endif

#include <inc/x86.h>
#include <inc/mmu.h>
#include <inc/error.h>
#include <inc/string.h>

#include <kern/helpers.h>
#include <kern/kclock.h>

#include <inc/memlayout.h>
#include <inc/assert.h>
struct Env;

extern char ptr_stack_top[], ptr_stack_bottom[];

extern struct Frame_Info *frames_info;
extern uint32 number_of_frames;


extern uint32 phys_page_directory;
extern uint32 *ptr_page_directory;

void boot_map_range(uint32 *ptr_page_directory, uint32 virtual_address, uint32 size, uint32 physical_address, int perm);
uint32* boot_get_page_table(uint32 *ptr_page_directory, uint32 virtual_address, int create);
void* boot_allocate_space(uint32 size, uint32 align);
void	initialize_kernel_VM();

void	initialize_paging();
int allocate_frame(struct Frame_Info **ptr_frame_info);
void free_frame(struct Frame_Info *ptr_frame_info);
int get_page_table(uint32 *ptr_page_directory, const void *virtual_address, int create, uint32 **ptr_page_table);
int	map_frame(uint32 *ptr_page_directory, struct Frame_Info *ptr_frame_info, void *virtual_address, int perm);
void	unmap_frame(uint32 *pgdir, void *va);
struct Frame_Info *get_frame_info(uint32 *ptr_page_directory, void *virtual_address, uint32 **ptr_page_table);
void decrement_references(struct Frame_Info* ptr_frame_info);

static inline uint32 to_frame_number(struct Frame_Info *ptr_frame_info)
{
	return ptr_frame_info - frames_info;
}

static inline uint32 to_physical_address(struct Frame_Info *ptr_frame_info)
{
	return to_frame_number(ptr_frame_info) << PGSHIFT;
}

static inline struct Frame_Info* to_frame_info(uint32 physical_address)
{
	if (PPN(physical_address) >= number_of_frames)
		panic("to_frame_info called with invalid pa");
	return &frames_info[PPN(physical_address)];
}


int get_page_table(uint32 *ptr_page_directory, const void *virtual_address, int create, uint32 **ptr_page_table);

void loadProcess(int id);
void initialize_process(int id);

uint32 calculate_free_frames();
uint32 calculate_required_frames(uint32* ptr_page_directory, uint32 start_virtual_address, uint32 size);
int get_page(uint32* ptr_page_directory, void *va, int perm);
void freeMem(uint32* ptr_page_directory, void *virtual_address, uint32 size);
#endif /* !FOS_KERN_MEM_MAN_H */
