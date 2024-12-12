#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <common.h>

void *guest_to_host(paddr_t addr);
paddr_t host_to_guest(const void *addr);

#endif