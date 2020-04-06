/* See COPYRIGHT for copyright information. */

#ifndef FOS_INC_ERROR_H
#define FOS_INC_ERROR_H

// Kernel error codes -- keep in sync with list in lib/printfmt.c.
#define E_UNSPECIFIED	-1	// Unspecified or unknown problem
#define E_BAD_ENV	-2	// Environment doesn't exist or otherwise
				// cannot be used in requested action
#define E_INVAL		-3	// Invalid parameter
#define E_NO_MEM	-4	// Request failed due to memory shortage
#define E_NO_FREE_ENV	-5	// Attempt to create a new environment beyond
				// the maximum allowed
#define E_FAULT		-6	// Memory fault
#define E_EOF		-7	// Unexpected end of file

#define E_NO_PAGE_FILE_SPACE -8	// No free space in page file for new pages
#define E_PAGE_NOT_EXIST_IN_PF  -9	// PAGE NOT EXIST  in page file
#define E_TABLE_NOT_EXIST_IN_PF  -10	// TABLE NOT EXIST  in page file
#define E_NO_SEMAPHORE	-11	// no system semaphres available
#define E_SEMAPHORE_EXISTS	-12
#define E_SEMAPHORE_NOT_EXISTS	-13
#define E_NO_SHARE	-14	// no system shared memory available
#define E_SHARED_MEM_EXISTS	-15	// shared memory var already exists
#define E_SHARED_MEM_NOT_EXISTS	-16	// shared memory var not exists
#define E_ENV_CREATION_ERROR	-17

#define E_NO_VM -20	// No free space in page file for new pages

#define	MAXERROR	100

#endif	// !FOS_INC_ERROR_H */
