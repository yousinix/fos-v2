#ifndef FOS_INC_TYPES_H
#define FOS_INC_TYPES_H

/// Screen logging definitions ===============================
#define LOG_ENABLED 0

#define LOG_STATMENT(statment) if (LOG_ENABLED) { statment; cprintf("\n"); }
#define LOG_VARS(string, cprintf_variable_arguments) if (LOG_ENABLED) { cprintf(string,cprintf_variable_arguments); cprintf("\n"); }
#define LOG_STRING(s) if (LOG_ENABLED) { cprintf(s); cprintf("\n");}
#define LOG_STATMENT_NO_NEWLINE(a) if (LOG_ENABLED) { a;}
///============================================================

#ifndef NULL
#define NULL ((void*) 0)
#endif

#define UINT_MAX 0xFFFFFFFF
// Represents true-or-false values
typedef int bool;

// Explicitly-sized versions of integer types
typedef __signed char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;

struct uint64
{
	uint32 low, hi;
};

// Pointers and addresses are 32 bits long.
// We use pointer types to represent virtual addresses,
// uintptr_t to represent the numerical values of virtual addresses,
// and physaddr_t to represent physical addresses.
//typedef int32_t intptr_t;
//typedef uint32_t uintptr_t;
//typedef uint32_t physaddr_t;

// Page numbers are 32 bits long.
//typedef uint32_t ppn_t;

// size_t is used for memory object sizes.
//typedef uint32_t size_t;
// ssize_t is a signed version of ssize_t, used in case there might be an
// error return.
//typedef int32_t ssize_t;

// off_t is used for file offsets and lengths.
//typedef int32_t off_t;

// Efficient min and max operations
#define MIN(_a, _b)						\
({								\
	typeof(_a) __a = (_a);					\
	typeof(_b) __b = (_b);					\
	__a <= __b ? __a : __b;					\
})
#define MAX(_a, _b)						\
({								\
	typeof(_a) __a = (_a);					\
	typeof(_b) __b = (_b);					\
	__a >= __b ? __a : __b;					\
})

// Rounding operations (efficient when n is a power of 2)
// Round down to the nearest multiple of n
#define ROUNDDOWN(a, n)						\
({								\
	uint32 __a = (uint32) (a);				\
	(typeof(a)) (__a - __a % (n));				\
})
// Round up to the nearest multiple of n
#define ROUNDUP(a, n)						\
({								\
	uint32 __n = (uint32) (n);				\
	(typeof(a)) (ROUNDDOWN((uint32) (a) + __n - 1, __n));	\
})

// Return the offset of 'member' relative to the beginning of a struct type
#define offsetof(type, member)  ((size_t) (&((type*)0)->member))

#endif /* !FOS_INC_TYPES_H */
