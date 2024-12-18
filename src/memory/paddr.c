#include <memory.h>
#include <device/mmio.h>

#define LIKELY_IN_MMIO(ADDR) \
    ((ADDR) < CONFIG_MBASE || (ADDR) >= CONFIG_MBASE + CONFIG_MSIZE)

__attribute__((always_inline))
word_t paddr_read(paddr_t addr, size_t len) {
    if (LIKELY_IN_MMIO(addr)) {
        return mmio_read(addr, len);
    }
    return host_read(guest_to_host(addr), len);
}

__attribute__((always_inline))
void paddr_write(paddr_t addr, size_t len, word_t data) {
    if (LIKELY_IN_MMIO(addr)) {
        mmio_write(addr, len, data);
    } else {
        host_write(guest_to_host(addr), len, data);
    }
}
