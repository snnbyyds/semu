#include <memory.h>

// TODO: Handle cases where we read / write in MMIO spaces

word_t paddr_read(paddr_t addr, size_t len) {
    return host_read(guest_to_host(addr), len);
}

void paddr_write(paddr_t addr, size_t len, word_t data) {
    host_write(guest_to_host(addr), len, data);
}
