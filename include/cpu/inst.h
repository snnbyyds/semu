#ifndef __INST_H__
#define __INST_H__

#include <common.h>

typedef struct {
    vaddr_t pc;
    vaddr_t snpc; // Static next PC
    vaddr_t dnpc; // Dynamic next PC
} exec_t;

typedef void (*decode_t)(uint32_t, exec_t *);

// Instruction decode masks
enum {
    // Rv32 Shared
    OPCODE       = 0b00000000000000000000000001111111,
    FUNCT3       = 0b00000000000000000111000000000000,
    FUNCT7       = 0b11111110000000000000000000000000,
    RD           = 0b00000000000000000000111110000000,
    RS1          = 0b00000000000011111000000000000000,
    RS2          = 0b00000001111100000000000000000000,
    INST_6_2     = 0b00000000000000000000000001111100,

    // R-type

    // I-type
    FI_IMM       = 0b11111111111100000000000000000000,

    // S-type
    FS_IMM_4_0   = 0b00000000000000000000111110000000,
    FS_IMM_11_5  = 0b11111110000000000000000000000000,

    // B-type
    FB_IMM_11    = 0b00000000000000000000000010000000,
    FB_IMM_4_1   = 0b00000000000000000000111100000000,
    FB_IMM_10_5  = 0b01111110000000000000000000000000,
    FB_IMM_12    = 0b10000000000000000000000000000000,

    // U-type
    FU_IMM_31_12 = 0b11111111111111111111000000000000,

    // J-type
    FJ_IMM_19_12 = 0b00000000000011111111000000000000,
    FJ_IMM_11    = 0b00000000000100000000000000000000,
    FJ_IMM_10_1  = 0b01111111111000000000000000000000,
    FJ_IMM_20    = 0b10000000000000000000000000000000,

    // R4-type
    FR4_FMT      = 0b00000110000000000000000000000000,
    FR4_RS3      = 0b11111000000000000000000000000000,

    // C-type
    FC_OPCODE    = 0b00000000000000000000000000000011,
    FC_FUNCT3    = 0b00000000000000001110000000000000,
    FC_RS1       = 0b00000000000000000000111110000000,
    FC_RS2       = 0b00000000000000000000000001111100,
    FC_RD        = 0b00000000000000000000111110000000,
};

__attribute__((always_inline))
static inline uint32_t decode_opcode(uint32_t inst) {
    return inst & OPCODE;
}

__attribute__((always_inline))
static inline uint32_t decode_rd(uint32_t inst) {
    return (inst & RD) >> 7;
}

__attribute__((always_inline))
static inline uint32_t decode_rs1(uint32_t inst) {
    return (inst & RS1) >> 15;
}

__attribute__((always_inline))
static inline uint32_t decode_rs2(uint32_t inst) {
    return (inst & RS2) >> 20;
}

__attribute__((always_inline))
static inline uint32_t decode_rs3(uint32_t inst) {
    return (inst & FR4_RS3) >> 27;
}

__attribute__((always_inline))
static inline uint32_t decode_funct3(uint32_t inst) {
    return (inst & FUNCT3) >> 12;
}

__attribute__((always_inline))
static inline uint32_t decode_funct7(uint32_t inst) {
    return (inst & FUNCT7) >> 25;
}

__attribute__((always_inline))
static inline int32_t decode_immI(uint32_t inst) {
    return ((int32_t)(inst & FI_IMM)) >> 20;
}

__attribute__((always_inline))
static inline int32_t decode_immS(uint32_t inst) {
    return (int32_t)((inst & FS_IMM_11_5) | ((inst & FS_IMM_4_0) << 13)) >> 20;
}

__attribute__((always_inline))
static inline int32_t decode_immB(uint32_t inst) {
    return (int32_t)((inst & FB_IMM_12) |
        ((inst & FB_IMM_11) << 23) |
        ((inst & FB_IMM_10_5) >> 1) |
        ((inst & FB_IMM_4_1) << 12)) >> 19;
}

__attribute__((always_inline))
static inline int32_t decode_immU(uint32_t inst) {
    return inst & FU_IMM_31_12;
}

__attribute__((always_inline))
static inline int32_t decode_immJ(uint32_t inst) {
    return (int32_t)((inst & FJ_IMM_20) |
        ((inst & FJ_IMM_19_12) << 11) |
        ((inst & FJ_IMM_11) << 2) |
        ((inst & FJ_IMM_10_1) >> 9)) >> 11;
}

__attribute__((always_inline))
static inline uint16_t c_decode_rs1(uint16_t inst) {
    return (uint16_t)((inst & FC_RS1) >> 7);
}

__attribute__((always_inline))
static inline uint16_t c_decode_rs2(uint16_t inst) {
    return (uint16_t)((inst & FC_RS2) >> 2);
}

__attribute__((always_inline))
static inline uint16_t c_decode_rd(uint16_t inst) {
    return (uint16_t)((inst & FC_RD) >> 7);
}

__attribute__((always_inline))
static inline uint16_t c_decode_rs1c(uint16_t inst) {
    return (uint16_t)((inst & 0x380) >> 7);
}

__attribute__((always_inline))
static inline uint16_t c_decode_rs2c(uint16_t inst) {
    return (uint16_t)((inst & 0x1c) >> 2);
}

__attribute__((always_inline))
static inline uint16_t c_decode_rdc(const uint16_t inst) {
    return (uint16_t)((inst & 0x1c) >> 2);
}

__attribute__((always_inline))
static inline uint16_t c_decode_caddi4spn_nzuimm(uint16_t inst) {
    return ((inst & 0x1800) >> 7) |
        ((inst & 0x780) >> 1) |
        ((inst & 0x40) >> 4) |
        ((inst & 0x20) >> 2);
}

__attribute__((always_inline))
static inline int32_t c_decode_caddi16sp_nzimm(uint16_t inst) {
    int32_t t = (inst & 0x1000) >> 3;
    t |= (inst & 0x40) >> 2;
    t |= (inst & 0x20) << 1;
    t |= (inst & 0x18) << 4;
    t |= (inst & 0x4) << 3;
    return (t & 0x200) ? (t | 0xfffffc00) : (uint32_t)t;
}

__attribute__((always_inline))
static inline uint32_t c_decode_clui_nzimm(uint16_t inst) {
    uint32_t t = (inst & 0x1000) << 5 | (inst & 0x7c) << 10;
    return (t & 0x20000) ? (t | 0xfffc0000) : t;
}

__attribute__((always_inline))
static inline int32_t c_decode_caddi_imm(uint16_t inst) {
    int32_t t = 0;
    uint16_t mask = (0x1000 & inst) << 3;
    t |= (mask >> 0);
    t |= (mask >> 1);
    t |= (mask >> 2);
    t |= (mask >> 3);
    t |= (mask >> 4);
    t |= (mask >> 5);
    t |= (mask >> 6);
    t |= (mask >> 7);
    t |= (mask >> 8);
    t |= (mask >> 9);
    t |= (mask >> 10);
    t |= (inst & 0x7c) >> 2;
    return (int32_t)(int16_t)t;
}

__attribute__((always_inline))
static inline int32_t c_decode_citype_imm(uint16_t inst) {
    uint32_t t = ((inst & 0x1000) >> 7) | ((inst & 0x7c) >> 2);
    return (t & 0x20) ? (int32_t)(t | 0xffffffc0) : (int32_t)t;
}

__attribute__((always_inline))
static inline int32_t c_decode_cjtype_imm(uint16_t inst) {
    uint16_t t = 0;

    t |= (inst & 0x38) >> 2;
    t |= (inst & 0x800) >> 7;
    t |= (inst & 0x4) << 3;
    t |= (inst & 0x80) >> 1;
    t |= (inst & 0x40) << 1;
    t |= (inst & 0x600) >> 1;
    t |= (inst & 0x100) << 2;
    t |= (inst & 0x1000) >> 1;

    t |= (0x800 & t) << 1;
    t |= (0x800 & t) << 2;
    t |= (0x800 & t) << 3;
    t |= (0x800 & t) << 4;

    return (int32_t)(int16_t)t;
}

__attribute__((always_inline))
static inline uint32_t c_decode_cslli_imm(uint16_t inst) {
    return ((inst & 0x1000) >> 7) |
        ((inst & 0x7c) >> 2);
}

__attribute__((always_inline))
static inline uint8_t c_decode_cbtype_shamt(uint16_t inst) {
    uint8_t t = 0;
    t |= (inst & 0x1000) >> 7;
    t |= (inst & 0x7c) >> 2;
    return t;
}

__attribute__((always_inline))
static inline uint16_t c_decode_cbtype_imm(uint16_t inst) {
    uint16_t t = 0;

    t |= (inst & 0x18) >> 2;
    t |= (inst & 0xc00) >> 7;
    t |= (inst & 0x4) << 3;
    t |= (inst & 0x60) << 1;
    t |= (inst & 0x1000) >> 4;

    t |= (0x100 & t) << 1;
    t |= (0x100 & t) << 2;
    t |= (0x100 & t) << 3;
    t |= (0x100 & t) << 4;
    t |= (0x100 & t) << 5;
    t |= (0x100 & t) << 6;
    t |= (0x100 & t) << 7;
    t |= (0x100 & t) << 8;

    return t;
}

#define decode_rm decode_funct3

#endif
