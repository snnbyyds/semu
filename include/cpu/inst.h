/*
 * Copyright 2025 Nuo Shen, Nanjing University
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __INST_H__
#define __INST_H__

#include <common.h>

typedef struct {
    vaddr_t pc;
    vaddr_t snpc; // Static next PC
    vaddr_t dnpc; // Dynamic next PC
} exec_t;

typedef void (*decode_t)(uint32_t, exec_t *restrict);

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
};

static inline uint32_t decode_opcode(uint32_t inst) {
    return inst & OPCODE;
}

static inline uint32_t decode_rd(uint32_t inst) {
    return (inst & RD) >> 7;
}

static inline uint32_t decode_rs1(uint32_t inst) {
    return (inst & RS1) >> 15;
}

static inline uint32_t decode_rs2(uint32_t inst) {
    return (inst & RS2) >> 20;
}

static inline uint32_t decode_rs3(uint32_t inst) {
    return (inst & FR4_RS3) >> 27;
}

static inline uint32_t decode_funct3(uint32_t inst) {
    return (inst & FUNCT3) >> 12;
}

static inline uint32_t decode_funct7(uint32_t inst) {
    return (inst & FUNCT7) >> 25;
}

static inline uint32_t decode_fmt(uint32_t inst) {
    return (inst & FR4_FMT) >> 25;
}

static inline int32_t decode_immI(uint32_t inst) {
    return ((int32_t)(inst & FI_IMM)) >> 20;
}

static inline int32_t decode_immS(uint32_t inst) {
    return (int32_t)((inst & FS_IMM_11_5) | ((inst & FS_IMM_4_0) << 13)) >> 20;
}

static inline int32_t decode_immB(uint32_t inst) {
    return (int32_t)((inst & FB_IMM_12) | ((inst & FB_IMM_11) << 23) | ((inst & FB_IMM_10_5) >> 1) | ((inst & FB_IMM_4_1) << 12)) >> 19;
}

static inline int32_t decode_immU(uint32_t inst) {
    return inst & FU_IMM_31_12;
}

static inline int32_t decode_immJ(uint32_t inst) {
    return (int32_t)((inst & FJ_IMM_20) | ((inst & FJ_IMM_19_12) << 11) | ((inst & FJ_IMM_11) << 2) | ((inst & FJ_IMM_10_1) >> 9)) >> 11;
}

#define decode_rm decode_funct3

#endif
