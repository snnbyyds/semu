#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <assert.h>

#include <debug.h>
#include <generated/autoconf.h>
#include <macro.h>

typedef uint32_t word_t;
typedef word_t vaddr_t;
typedef vaddr_t paddr_t;

#define PRIword PRIu32
#define PRIaddr PRIx32

#define PGSIZE 4096
#define PG_ALIGN __attribute((aligned(PGSIZE)))

#endif
