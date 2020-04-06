/* See COPYRIGHT for copyright information. */

#ifndef FOS_INC_ASSERT_H
#define FOS_INC_ASSERT_H

#include <inc/stdio.h>

void _warn(const char*, int, const char*, ...);
void _panic(const char*, int, const char*, ...) __attribute__((noreturn));
void _panic_all(const char*, int, const char*, ...) __attribute__((noreturn));
void _panic_into_prompt(const char*, int, const char*, ...) __attribute__((noreturn));

#define warn(...) _warn(__FILE__, __LINE__, __VA_ARGS__)
#define panic(...) _panic(__FILE__, __LINE__, __VA_ARGS__)
#define kpanic_all(...) _panic_all(__FILE__, __LINE__, __VA_ARGS__)
#define kpanic_into_prompt(...) _panic_into_prompt(__FILE__, __LINE__, __VA_ARGS__)

#define assert(x)		\
	do { if (!(x)) panic("assertion failed: %s", #x); } while (0)

#define assert_endall(x)		\
	do { if (!(x)) kpanic_all("assertion failed: %s", #x); } while (0)

// static_assert(x) will generate a compile-time error if 'x' is false.
#define static_assert(x)	switch (x) case 0: case (x):

#endif /* !FOS_INC_ASSERT_H */
