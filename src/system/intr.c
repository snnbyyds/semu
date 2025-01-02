#include <cpu/cpu.h>
#include <cpu/reg.h>

void etrace(word_t no, vaddr_t epc);

word_t isa_raise_intr(word_t no, vaddr_t epc) {
    etrace(no, epc);
    csr(MEPC) = epc;    /* SR[mepc] <- PC */
    csr(MCAUSE) = no;   /* SR[mcause] <- NO */
    return csr(MTVEC);  /* SR[mtvec] */
}
