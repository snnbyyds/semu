#ifndef __MMIO_H__
#define __MMIO_H__

#include <common.h>

#define MMIO_WRITE true
#define MMIO_READ false

#define LIKELY_IN_MMIO(ADDR) \
    ((ADDR) < CONFIG_MBASE || (ADDR) >= CONFIG_MBASE + CONFIG_MSIZE)

typedef bool mmio_rw_t;
typedef void (*io_handler_t) (mmio_rw_t mmio_rw_op, size_t offset, size_t len);

typedef struct {
    ioaddr_t low;
    ioaddr_t high;
    io_handler_t io_handler;
    void *space;
} IOMap;

void *add_iomap(ioaddr_t start, size_t size, io_handler_t handler);
word_t mmio_read(ioaddr_t addr, size_t len);
void mmio_write(ioaddr_t addr, size_t len, word_t data);
void init_mmio();

#endif
