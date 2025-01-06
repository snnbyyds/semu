#include <cpu/cpu.h>
#include <cpu/reg.h>
#include <device/timer.h>

void etrace(word_t no, vaddr_t epc);

word_t isa_query_intr() {
    if (cpu.intr && get_csr_field(MSTATUS, MSTATUS_MIE, MSTATUS_MIE_SHIFT)) {
        cpu.intr = false;
        return IRQ_TIMER;
    }
    return -1;
}

word_t isa_raise_intr(word_t no, vaddr_t epc) {
    etrace(no, epc);

    // Put the CPU to interrupt-disable state
    set_csr_field(MSTATUS, MSTATUS_MPIE, MSTATUS_MPIE_SHIFT,
        get_csr_field(MSTATUS, MSTATUS_MIE, MSTATUS_MIE_SHIFT));
    set_csr_field(MSTATUS, MSTATUS_MIE, MSTATUS_MIE_SHIFT, 0);

    csr(MEPC) = epc;    /* SR[mepc] <- PC */
    csr(MCAUSE) = no;   /* SR[mcause] <- NO */
    return csr(MTVEC);  /* SR[mtvec] */
}
