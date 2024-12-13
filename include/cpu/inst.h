#ifndef __INST_H__
#define __INST_H__

#include <common.h>

typedef union __attribute__((packed)) {
    uint32_t raw_inst;
    struct { uint32_t opcode : 7, raw_data : 25; };
    // Decode schemes
    struct { uint32_t opcode : 7, rd : 5, funct3 : 3, rs1 : 5, rs2 : 5, funct7 : 7; } R_type;
    struct { uint32_t opcode : 7, rd : 5, funct3 : 3, rs1 : 5, imm : 12; } I_type;
    struct { uint32_t opcode : 7, imm4_0 : 5, funct3 : 3, rs1 : 5, rs2 : 5, imm11_5 : 7; } S_type;
    struct { uint32_t opcode : 7, imm11 : 1, imm4_1 : 4, funct3 : 3, rs1 : 5, rs2 : 5, imm10_5 : 6, imm12 : 1; } B_type;
    struct { uint32_t opcode : 7, rd : 5, imm31_12 : 20; } U_type;
    struct { uint32_t opcode : 7, rd : 5, imm19_12 : 8, imm11 : 1, imm10_1 : 10, imm20 : 1; } J_type;
} inst_t;

typedef struct {
    vaddr_t pc;
    vaddr_t snpc; // Static next PC
    vaddr_t dnpc; // Dynamic next PC
    inst_t inst;
} exec_t;

#endif
