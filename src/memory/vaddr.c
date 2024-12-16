#include <memory.h>

// Do translation if MMU is ON
word_t vaddr_read(vaddr_t addr, size_t len) {
    if (MMU_STATUS() == MMU_ON) {
        return paddr_read(MMU_TRANSLATE(addr, MEM_TYPE_READ), len);
    }
    return paddr_read(addr, len);
}

void vaddr_write(vaddr_t addr, size_t len, word_t data) {
    if (MMU_STATUS() == MMU_ON) {
        paddr_write(MMU_TRANSLATE(addr, MEM_TYPE_WRITE), len, data);
    } else {
        paddr_write(addr, len, data);
    }
}

word_t vaddr_fetchinst(vaddr_t addr, size_t len) {
    if (MMU_STATUS() == MMU_ON) {
        return paddr_read(MMU_TRANSLATE(addr, MEM_TYPE_EXEC), len);
    }
    return paddr_read(addr, len);
}
