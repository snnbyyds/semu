#include <memory.h>

// Since we have not implemented paging yet, so just treat vaddr as paddr

word_t vaddr_read(vaddr_t addr, size_t len) {
    return paddr_read(addr, len);
}

void vaddr_write(vaddr_t addr, size_t len, word_t data) {
    paddr_write(addr, len, data);
}
