#include <cpu/cpu.h>
#include <cpu/reg.h>
#include <memory.h>

// Translate vaddr to paddr if paging is ON

__attribute__((always_inline))
word_t vaddr_read(vaddr_t addr, size_t len) {
    if (MMU_CHECK() == MMU_ON) {
        return paddr_read(mmu_translate(addr, MEM_TYPE_READ), len);
    }
    return paddr_read(addr, len);
}

__attribute__((always_inline))
void vaddr_write(vaddr_t addr, size_t len, word_t data) {
    if (MMU_CHECK() == MMU_ON) {
        paddr_write(mmu_translate(addr, MEM_TYPE_WRITE), len, data);
    } else {
        paddr_write(addr, len, data);
    }
}

__attribute__((always_inline))
word_t vaddr_fetchinst(vaddr_t addr, size_t len) {
    if (MMU_CHECK() == MMU_ON) {
        return paddr_read(mmu_translate(addr, MEM_TYPE_IFETCH), len);
    }
    return paddr_read(addr, len);
}
