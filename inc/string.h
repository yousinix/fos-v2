#ifndef FOS_INC_STRING_H
#define FOS_INC_STRING_H

#include <inc/types.h>

#define MAX_ARGUMENTS 16

int	strlen(const char *s);
int	strnlen(const char *s, uint32 size);
char *	strcpy(char *dst, const char *src);
char *	strncpy(char *dst, const char *src, uint32 size);
uint32	strlcpy(char *dst, const char *src, uint32 size);
int	strcmp(const char *s1, const char *s2);
int	strncmp(const char *s1, const char *s2, uint32 size);
char *	strchr(const char *s, char c);
char *	strfind(const char *s, char c);

void *	memset(void *dst, int c, uint32 len);
void *	memcpy(void *dst, const void *src, uint32 len);
void *	memmove(void *dst, const void *src, uint32 len);
int	memcmp(const void *s1, const void *s2, uint32 len);
void *	memfind(const void *s, int c, uint32 len);

long	strtol(const char *s, char **endptr, int base);

int strsplit(char *string, char *SPLIT_CHARS, char **argv, int * argc);

#endif /* not FOS_INC_STRING_H */
