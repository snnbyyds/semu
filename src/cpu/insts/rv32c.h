#ifndef __RV32C_H__
#define __RV32C_H__

#include <cpu/fpu.h>
#include <utils/state.h>
#include <system.h>
#include "macros.h"

extern decode_t unimpl;

static inline void op_caddi4spn(uint32_t inst, exec_t *info) {
    /* c.addi4spn */
    R(c_decode_rdc(inst) | 0x8) =
        R(2) + c_decode_caddi4spn_nzuimm(inst); // R(2) is $sp
}

static inline void op_caddi(uint32_t inst, exec_t *info) {
    const uint16_t __rd = c_decode_rd(inst);
    if (likely(__rd)) {
        /* c.addi */
        R(__rd) += (int16_t)c_decode_citype_imm(inst);
    } else {
        /* c.nop */
        return;
    }
}

static inline void op_cslli(uint32_t inst, exec_t *info) {
    /* c.slli */
    R(c_decode_rd(inst)) <<= (uint8_t)c_decode_cslli_imm(inst);
}

static inline void op_cjal(uint32_t inst, exec_t *info) {
    /* c.jal */
    R(1) = PC + 2;
    NPC = PC + (uint32_t)(int32_t)(int16_t)c_decode_cjtype_imm(inst);
}

static inline void op_clw(uint32_t inst, exec_t *info) {
    /* c.lw */
    register uint16_t imm = 0;
    imm |= (inst & 0x40) >> 4;
    imm |= (inst & 0x1c00) >> 7;
    imm |= (inst & 0x20) << 1;
    const uint32_t __rd = c_decode_rdc(inst) | 0x8;
    const uint32_t __rs1 = c_decode_rs1c(inst) | 0x8;

    R(__rd) = vaddr_read_w(R(__rs1) + (uint32_t)imm);
}

static inline void op_cli(uint32_t inst, exec_t *info) {
    /* c.li */
    R(c_decode_rd(inst)) = c_decode_citype_imm(inst);
}

static inline void op_clwsp(uint32_t inst, exec_t *info) {
    /* c.lwsp */
    uint16_t imm = 0;
    imm |= (inst & 0x70) >> 2;
    imm |= (inst & 0xc) << 4;
    imm |= (inst & 0x1000) >> 7;
    R(c_decode_rd(inst)) = vaddr_read_w(R(2) + imm); // R(2) is $sp
}

static inline void op_cflw(uint32_t inst, exec_t *info) {
    /* c.flw */
    register uint16_t imm = 0;
    imm |= (inst & 0x40) >> 4;
    imm |= (inst & 0x1c00) >> 7;
    imm |= (inst & 0x20) << 1;
    F(c_decode_rdc(inst) | 0x08).v =
        vaddr_read_w(R(c_decode_rs1c(inst) | 0x08) +
            (uint32_t)imm);
}

static inline void op_clui(uint32_t inst, exec_t *info) {
    const uint16_t __rd = c_decode_rd(inst);
    switch (__rd) {
        case 0: /* c.nop */
            break;
        case 2: /* c.addi16sp */
            R(2) += c_decode_caddi16sp_nzimm(inst); // R(2) is $sp
            break;
        default: /* c.lui */
            R(__rd) = c_decode_clui_nzimm(inst);
            break;
    }
}

static inline void op_cflwsp(uint32_t inst, exec_t *info) {
    uint16_t imm = 0;
    imm |= (inst & 0x70) >> 2;
    imm |= (inst & 0xc) << 4;
    imm |= (inst & 0x1000) >> 7;
    F(c_decode_rd(inst)).v = vaddr_read_w(R(2) + imm); // R(2) is $sp
}

static inline void op_cmisc_alu(uint32_t inst, exec_t *info) {
    const uint32_t funct2 = (inst & 0xc00) >> 10;
    uint32_t __shamt, __rs1, __rs2;
    switch (funct2) {
        case 0: /* c.srli */
            __shamt = 0;
            __shamt |= (inst & 0x1000) >> 7;
            __shamt |= (inst & 0x7c) >> 2;
            if (unlikely(__shamt & 0x20)) {
                unimpl(inst, info);
                break;
            }
            __rs1 = c_decode_rs1c(inst) | 0x8;
            if (!__rs1 || !__shamt) {
                break; /* c.nop */
            }
            R(__rs1) >>= __shamt;
            break;
        case 1: /* c.srai */
            __shamt = 0;
            __shamt |= (inst & 0x1000) >> 7;
            __shamt |= (inst & 0x7c) >> 2;
            if (unlikely(__shamt & 0x20)) {
                unimpl(inst, info);
                break;
            }
            __rs1 = c_decode_rs1(inst);
            const uint32_t mask = R(__rs1) & 0x80000000;
            R(__rs1) >>= __shamt;
            for (uint32_t i = 0; i < __shamt; i++) {
                R(__rs1) |= mask >> i;
            }
            break;
        case 2: /* c.andi */
            __rs1 = c_decode_rs1c(inst) | 0x8;
            R(__rs1) &= c_decode_caddi_imm(inst);
            break;
        case 3:
            __rs1 = c_decode_rs1c(inst) | 0x8;
            __rs2 = c_decode_rs2c(inst) | 0x8;
            const uint32_t __rd = __rs1;
            switch (((inst & 0x1000) >> 10) | ((inst & 0x0060) >> 5)) {
                case 0: /* c.sub */
                    R(__rd) -= R(__rs2);
                    break;
                case 1: /* c.xor */
                    R(__rd) ^= R(__rs2);
                    break;
                case 2: /* c.or */
                    R(__rd) |= R(__rs2);
                    break;
                case 3: /* c.and */
                    R(__rd) &= R(__rs2);
                    break;
                default:
                    unimpl(inst, info);
                    break;
            }
            break;
        default:
            unimpl(inst, info);
            break;
    }
}

static inline void op_ccr(uint32_t inst, exec_t *info) {
    const uint32_t __rs1 = c_decode_rs1(inst);
    const uint32_t __rs2 = c_decode_rs2(inst);
    const uint32_t __rd = __rs1;

    switch ((inst & 0x1000) >> 12) {
        case 0:
            switch (__rs2) {
                case 0: /* c.jr */
                    if (!__rs1) {
                        break; /* c.nop */
                    }
                    NPC = R(__rs1);
                    break;
                default: /* c.mv */
                    if (!__rd) {
                        break; /* c.nop */
                    }
                    R(__rd) = R(__rs2);
                    break;
            }
            break;
        case 1:
            if (!__rs1 && !__rs2) { /* c.ebreak */
                SET_STATE(gpr(10) ? ABORT : END);
            } else if (__rs1 && __rs2) { /* c.add */
                R(__rd) += R(__rs2);
            } else if (__rs1 && !__rs2) { /* c.jalr */
                uint32_t t = PC + 2;
                NPC = R(__rs1);
                R(1) = t;
            }
            /* c.nop */
            break;
        default:
            __builtin_unreachable();
            break;
    }
}

static inline void op_cj(uint32_t inst, exec_t *info) {
    /* c.j */
    NPC = PC + c_decode_cjtype_imm(inst);
}

static inline void op_csw(uint32_t inst, exec_t *info) {
    /* c.sw */
    register uint32_t imm = 0;

    imm |= (inst & 0x40) >> 4;
    imm |= (inst & 0x1c00) >> 7;
    imm |= (inst & 0x20) << 1;

    vaddr_write_w(R((c_decode_rs1c(inst) | 0x8) + imm),
        R(c_decode_rs2c(inst) | 0x8));
}

static inline void op_cbeqz(uint32_t inst, exec_t *info) {
    /* c.beqz */
    if (!R(c_decode_rs1c(inst) | 0x8)) {
        NPC = PC + (uint32_t)(int32_t)(int16_t)c_decode_cbtype_imm(inst);
    }
}

static inline void op_cswsp(uint32_t inst, exec_t *info) {
    /* c.swsp */
    const uint32_t imm = (inst & 0x1e00) >> 7 | (inst & 0x180) >> 1;
    vaddr_write_w(R(2) + imm, c_decode_rs2(inst));
}

static inline void op_cfsw(uint32_t inst, exec_t *info) {
    /* c.fsw */
    register uint32_t imm = 0;
    imm |= (inst & 0x40) >> 4;
    imm |= (inst & 0x1c00) >> 7;
    imm |= (inst & 0x20) << 1;
    vaddr_write_w(R(c_decode_rs1c(inst) | 0x8) + imm,
        F(c_decode_rs2c(inst) | 0x8).v);
}

static inline void op_cbnez(uint32_t inst, exec_t *info) {
    /* c.bnez */
    if (R(c_decode_rs1c(inst) | 0x8)) {
        NPC = PC + (uint32_t)(int32_t)(int16_t)c_decode_cbtype_imm(inst);
    }
}

static inline void op_cfswsp(uint32_t inst, exec_t *info) {
    /* c.fswsp */
    const uint32_t imm = (inst & 0x1e00) >> 7 | (inst & 0x180) >> 1;
    vaddr_write_w(R(2) + imm, F(c_decode_rs2(inst)).v);
}

#endif
