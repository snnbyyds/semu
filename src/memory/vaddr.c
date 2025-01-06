#include <memory.h>

word_t vaddr_read(vaddr_t addr, size_t len) {
    if (isa_mmu_check() == MMU_DIRECT) {
        return paddr_read(addr, len);
    }
    return paddr_read(isa_mmu_translate(addr, MEM_TYPE_READ), len);
}

void vaddr_write(vaddr_t addr, size_t len, word_t data) {
    if (isa_mmu_check() == MMU_DIRECT) {
        paddr_write(addr, len, data);
        return;
    }
    paddr_write(isa_mmu_translate(addr, MEM_TYPE_WRITE), len, data);
}

word_t vaddr_ifetch(vaddr_t addr, size_t len) {
    if (isa_mmu_check() == MMU_DIRECT) {
        return paddr_read(addr, len);
    }
    return paddr_read(isa_mmu_translate(addr, MEM_TYPE_IFETCH), len);
}
