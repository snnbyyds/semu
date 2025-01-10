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

#include <softfloat.h>

#ifndef VERSION
#define VERSION 0.0
#endif

typedef float32_t rv_float_t;

typedef uint32_t word_t;
typedef int32_t sword_t;

typedef word_t paddr_t;
typedef paddr_t vaddr_t;
typedef word_t ioaddr_t;

#define PRIword PRIu32
#define PRIaddr PRIx32

#define PGSIZE 4096
#define PG_ALIGN __attribute((aligned(PGSIZE)))

#define __EXPORT __attribute__((visibility("default")))

#endif
