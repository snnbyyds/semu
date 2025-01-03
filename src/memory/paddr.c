#include <memory.h>
#include <device/mmio.h>

word_t paddr_read(paddr_t addr, size_t len) {
    if (LIKELY_IN_MMIO(addr)) {
        return mmio_read(addr, len);
    }
    return host_read(guest_to_host(addr), len);
}

void paddr_write(paddr_t addr, size_t len, word_t data) {
    if (LIKELY_IN_MMIO(addr)) {
        mmio_write(addr, len, data);
    } else {
        host_write(guest_to_host(addr), len, data);
    }
}
