#include <memory.h>
#include <unistd.h>
#include <fcntl.h>

static uint8_t memory[CONFIG_MSIZE] PG_ALIGN;

static inline paddr_t check_guestaddr(paddr_t addr) {
    assert(addr >= CONFIG_MBASE && addr < CONFIG_MBASE + CONFIG_MSIZE);
    return addr;
}

void *guest_to_host(paddr_t addr) {
    return memory + (check_guestaddr(addr) - CONFIG_MBASE);
}

paddr_t host_to_guest(const void *addr) {
    return check_guestaddr(addr - (const void *)memory + CONFIG_MBASE);
}

word_t host_read(const haddr_t haddr, size_t len) {
    switch (len) {
        case 1: return *(const uint8_t *)haddr;
        case 2: return *(const uint16_t *)haddr;
        case 4: return *(const uint32_t *)haddr;
        default: Warn("Invalid len: %zu", len); assert(0);
    }
    return -1;
}

void host_write(haddr_t haddr, size_t len, word_t data) {
    switch (len) {
        case 1: *(uint8_t *)haddr = (uint8_t)data; break;
        case 2: *(uint16_t *)haddr = (uint16_t)data; break;
        case 4: *(uint32_t *)haddr = (uint32_t)data; break;
        default: Warn("Invalid len: %zu", len); assert(0);
    }
}

void init_memory() {
    int urandom_fd = open("/dev/urandom", O_RDONLY);
    assert(urandom_fd != -1);
    ssize_t nread = read(urandom_fd, memory, CONFIG_MSIZE);
    assert(nread == CONFIG_MSIZE);
    close(urandom_fd);
    Log("Memory initialized at 0x%" PRIaddr "", (paddr_t)CONFIG_MBASE);
}
