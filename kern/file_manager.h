#ifndef FOS_KERN_FILE_MAN_H
#define FOS_KERN_FILE_MAN_H
#ifndef FOS_KERNEL
# error "This is a FOS kernel header; user programs should not #include it"
#endif

#include <inc/x86.h>

#define SECTOR_SIZE 512
#define PAGE_FILE_START_SECTOR ( (20<<20) /SECTOR_SIZE)  //start sector number of Page file in H.D.
#define SECTOR_PER_PAGE (PAGE_SIZE/SECTOR_SIZE)

#define PAGE_FILE_SIZE (520 << 20)   	//page file size in MB
#define PAGES_PER_FILE (PAGE_FILE_SIZE/PAGE_SIZE)

///=============================================================================================

int pf_add_empty_env_page( struct Env* ptr_env, uint32 virtual_address, uint8 initializeByZero);
int pf_update_env_page(struct Env* ptr_env, void *virtual_address, struct Frame_Info* modified_page_frame_info);
//int pf_special_update_env_modified_page(struct Env* ptr_env, uint32 virtual_address, struct Frame_Info* page_modified_frame_info);
int pf_read_env_page(struct Env* ptr_env, void *virtual_address);
void pf_remove_env_page(struct Env* ptr_env, uint32 virtual_address);
int pf_add_env_page( struct Env* ptr_env, uint32 virtual_address, void* dataSrc);
///=============================================================================================

int pf_calculate_allocated_pages(struct Env* ptr_env);
void pf_free_env(struct Env* ptr_env);
void scarce_memory();
#endif //FOS_KERN_FILE_MAN_H
