#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <common.h>

typedef void * haddr_t;

void *guest_to_host(paddr_t addr);
paddr_t host_to_guest(const void *addr);

word_t host_read(const haddr_t haddr, size_t len);
void host_write(haddr_t haddr, size_t len, word_t data);

word_t paddr_read(paddr_t addr, size_t len);
void paddr_write(paddr_t addr, size_t len, word_t data);

word_t vaddr_read(vaddr_t addr, size_t len);
void vaddr_write(vaddr_t write, size_t len, word_t data);

#endif