#ifndef __MACRO_H__
#define __MACRO_H__

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define ROUNDUP(n, size)   ((((uintptr_t)n) + (size) - 1) & ~((size) - 1))
#define ROUNDDOWN(n, size) ((((uintptr_t)n)) & ~((size) - 1))

#define LENGTH(arr) (sizeof(arr) / sizeof((arr)[0]))

#endif
