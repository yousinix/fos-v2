/* See COPYRIGHT for copyright information. */
#ifndef FOS_KERN_HELPER_H
#define FOS_KERN_HELPER_H
#ifndef FOS_KERNEL
# error "This is a FOS kernel header; user programs should not #include it"
#endif

#include <kern/user_environment.h>

extern struct Segdesc gdt[];
extern struct Pseudodesc gdt_pd;

extern char ptr_stack_top[], ptr_stack_bottom[];

// These variables are set by detect_memory()
//uint32 maxpa;	// Maximum physical address
uint32 number_of_frames;	// Amount of physical memory (in frames)
//uint32 size_of_base_mem;		// Amount of base memory (in bytes)
//uint32 size_of_extended_mem;		// Amount of extended memory (in bytes)

extern uint32* ptr_page_directory;
extern uint32 phys_page_directory;
extern char* ptr_free_mem;

extern struct Frame_Info *frames_info;
extern struct Linked_List free_frame_list;	// Free list of physical frames
extern struct Linked_List modified_frame_list;	// Free list of physical frames
extern uint32 number_of_frames;

/* This macro takes a user supplied address and turns it into
 * something that will cause a fault if it is a kernel address.  ULIM
 * itself is guaranteed never to contain a valid page.
 */
#define TRUP(_p)   						\
({								\
	register typeof((_p)) __m_p = (_p);			\
	(uint32) __m_p > ULIM ? (typeof(_p)) ULIM : __m_p;	\
})

/* This macro takes a kernel virtual address -- an address that points above
 * KERNEL_BASE, where the machine's maximum 256MB of physical memory is mapped --
 * and returns the corresponding physical address.  It panics if you pass it a
 * non-kernel virtual address.
 */
#define STATIC_KERNEL_PHYSICAL_ADDRESS(kva)						\
({								\
	uint32 __m_kva = (uint32) (kva);		\
	if (__m_kva < KERNEL_BASE)					\
		panic("K_PHYSICAL_ADDRESS called with invalid kva %08lx", __m_kva);\
	__m_kva - KERNEL_BASE;					\
})

/* This macro takes a physical address and returns the corresponding kernel
 * virtual address.  It panics if you pass an invalid physical address. */
#define STATIC_KERNEL_VIRTUAL_ADDRESS(pa)						\
({								\
	uint32 __m_pa = (pa);				\
	uint32 __m_ppn = PPN(__m_pa);				\
	if (__m_ppn >= number_of_frames)					\
		panic("K_VIRTUAL_ADDRESS called with invalid pa %08lx", __m_pa);\
	(void*) (__m_pa + KERNEL_BASE);				\
})

/* This Macro creates a page table/direcotry entry (32 bits)
* according to the format of Intel page table/diretory entry
*/
#define CONSTRUCT_ENTRY(phys_frame_address, permissions) \
( \
	phys_frame_address | permissions \
)


void	detect_memory();
void 	turn_on_paging();
//void	page_check();
void	tlb_invalidate(uint32 *pgdir, void *ptr);
void	check_boot_pgdir();
void	setup_listing_to_all_page_tables_entries();
int envid2env(int32  envid, struct Env **env_store, bool checkperm);

#endif /* !FOS_KERN_HELPER_H */
