#include <cpu/cpu.h>
#include <cpu/reg.h>

static const char *gprs[] = {
    "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
    "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
    "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
    "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

#define DECLARE_NEW_CSR(NAME, IMPLEMENT) [NAME] = { tostr(NAME), NAME, IMPLEMENT },

static const CSR_Property csrs[4096] = {
    DECLARE_NEW_CSR(satp, true)
    DECLARE_NEW_CSR(mstatus, true)
    DECLARE_NEW_CSR(mtvec, true)
    DECLARE_NEW_CSR(mscratch, true)
    DECLARE_NEW_CSR(mepc, true)
    DECLARE_NEW_CSR(mcause, true)
};

void reg_val_display() {
    for (size_t i = 0; i < LENGTH(gprs); i++) {
        printf("%-4s\t0x%08x\t%" PRIword "\n", gprs[i], gpr(i), gpr(i));
    }
    putchar('\n');
    for (size_t i = 0; i < LENGTH(csrs); i++) {
        if (csrs[i].implemented) {
            printf("%-9s\t0x%03x\t%" PRIword "\n", csrs[i].name, csrs[i].number, csr(i));
        }
    }
}
