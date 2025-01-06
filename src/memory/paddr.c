#include <memory.h>
#include <device/mmio.h>

/* Read physical address */
word_t paddr_read(paddr_t addr, size_t len) {
    if (likely(IN_PMEM(addr))) {
        return host_read(guest_to_host(addr), len);
    }
    return mmio_read(addr, len);
}

/* Write physical address */
void paddr_write(paddr_t addr, size_t len, word_t data) {
    if (likely(IN_PMEM(addr))) {
        host_write(guest_to_host(addr), len, data);
    } else {
        mmio_write(addr, len, data);
    }
}
