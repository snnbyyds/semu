#include <cpu/cpu.h>
#include <cpu/reg.h>
#include <string.h>

static const char *regs[] = {
    "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
    "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
    "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
    "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

void reg_val_display() {
    for (size_t i = 0; i < LENGTH(regs); i++) {
        printf("%-4s\t0x%08x\t%" PRIword "\n", regs[i], gpr(i), gpr(i));
    }
}

char *reg_val_to_name(size_t idx) {
    static char buffer[4];
    assert(idx < LENGTH(regs));
    strncpy(buffer, regs[idx], 4);
    return buffer;
}
