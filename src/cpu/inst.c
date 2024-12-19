#include <cpu/cpu.h>
#include <cpu/inst.h>
#include <cpu/reg.h>
#include <memory.h>
#include <utils/state.h>
#include <string.h>
#include <assert.h>

#define R(i) gpr(i)
#define Mr vaddr_read
#define Mw vaddr_write
#define PC _info->pc
#define NPC _info->dnpc

typedef struct {
    void (*handler)();
} funct7_item;

typedef struct {
    funct7_item funct7_pool[0x80];
    void (*handler)();
} funct3_item;

typedef struct {
    funct3_item funct3_pool[0x8];
    void (*handler)();
} opcode_item;

static opcode_item pool[0x80];
static inst_t inst;
static exec_t *_info = NULL;

#define DECODE_R() \
    uint32_t rd __attribute__((unused)) = inst.R_type.rd, \
    rs1 __attribute__((unused)) = inst.R_type.rs1, \
    rs2 __attribute__((unused)) = inst.R_type.rs2;

#define DECODE_I() \
    uint32_t rd __attribute__((unused)) = inst.I_type.rd, \
    rs1 __attribute__((unused)) = inst.I_type.rs1; \
    word_t imm __attribute__((unused)) = SEXT(inst.I_type.imm, 12); \
    uint32_t inst31_26 __attribute__((unused)) = (imm >> 6) & 0x3f; \
    uint32_t shamt __attribute__((unused)) = imm & 0x3f;

#define DECODE_S() \
    uint32_t rs1 __attribute__((unused)) = inst.S_type.rs1, \
    rs2 __attribute__((unused)) = inst.S_type.rs2; \
    word_t imm __attribute__((unused)) = (SEXT(inst.S_type.imm11_5, 7) << 5) | inst.S_type.imm4_0; 

#define DECODE_B() \
    uint32_t rs1 __attribute__((unused)) = inst.B_type.rs1, \
    rs2 __attribute__((unused)) = inst.B_type.rs2; \
    word_t imm __attribute__((unused)) = SEXT(inst.B_type.imm12 << 12 | inst.B_type.imm11 << 11 | inst.B_type.imm10_5 << 5 | inst.B_type.imm4_1 << 1, 13);

#define DECODE_U() \
    uint32_t rd __attribute__((unused)) = inst.U_type.rd; \
    word_t imm __attribute__((unused)) = SEXT(inst.U_type.imm31_12, 20) << 12; 

#define DECODE_J() \
    uint32_t rd __attribute__((unused)) = inst.J_type.rd; \
    word_t imm __attribute__((unused)) = SEXT(inst.J_type.imm20 << 20 | inst.J_type.imm19_12 << 12 | inst.J_type.imm11 << 11 | inst.J_type.imm10_1 << 1, 21);

#define INST_EXEC(NAME, TYPE, ...) \
    static void __ ## NAME() { \
        DECODE_ ## TYPE(); \
        __VA_ARGS__ ; \
    }

#define _____ -1
#define _________ -1

#define RULE(NAME, TYPE, FUNCT7, FUNCT3, OPCODE) \
    do { \
        if (FUNCT3 == _____) { \
            pool[OPCODE].handler = __ ## NAME; \
            break; \
        } \
        if (FUNCT7 == _________) { \
            pool[OPCODE].funct3_pool[FUNCT3].handler = __ ## NAME; \
            break; \
        } \
        pool[OPCODE].funct3_pool[FUNCT3].funct7_pool[FUNCT7].handler = __ ## NAME; \
    } while (0)

// special handler for ones that can be matched with raw inst...
static inline void __special_handler() {
    switch (inst.raw_inst) {
        case 0b00000000000100000000000001110011: SET_STATE(END); break; // ebreak
        default: break;
    }
}

INST_EXEC(add,    R, R(rd) = R(rs1) + R(rs2))
INST_EXEC(and,    R, R(rd) = R(rs1) & R(rs2))
INST_EXEC(div,    R, R(rd) = (sword_t)R(rs1) / (sword_t)R(rs2))
INST_EXEC(divu,   R, R(rd) = R(rs1) / R(rs2))
INST_EXEC(mul,    R, R(rd) = R(rs1) * R(rs2))
INST_EXEC(mulh,   R, R(rd) = (int64_t)(sword_t)R(rs1) * (int64_t)(sword_t)R(rs2) >> 32LL)
INST_EXEC(mulhu,  R, R(rd) = (uint64_t)R(rs1) * (uint64_t)R(rs2) >> 32ULL)
INST_EXEC(or,     R, R(rd) = R(rs1) | R(rs2))
INST_EXEC(rem,    R, R(rd) = (sword_t)R(rs1) % (sword_t)R(rs2))
INST_EXEC(remu,   R, R(rd) = R(rs1) % R(rs2))
INST_EXEC(sll,    R, R(rd) = R(rs1) << R(rs2))
INST_EXEC(slt,    R, R(rd) = (sword_t)R(rs1) < (sword_t)R(rs2))
INST_EXEC(sltu,   R, R(rd) = R(rs1) < R(rs2))
INST_EXEC(sra,    R, R(rd) = (sword_t)R(rs1) >> (R(rs2) & 0x1f))
INST_EXEC(srl,    R, R(rd) = R(rs1) >> (R(rs2) & 0x1f))
INST_EXEC(sub,    R, R(rd) = R(rs1) - R(rs2))
INST_EXEC(xor,    R, R(rd) = R(rs1) ^ R(rs2))
INST_EXEC(addi,   I, R(rd) = R(rs1) + imm)
INST_EXEC(andi,   I, R(rd) = R(rs1) & imm)
INST_EXEC(ebreak, I, __special_handler())
INST_EXEC(jalr,   I, R(rd) = PC + 4, NPC = (R(rs1) + imm) & ~1)
INST_EXEC(lb,     I, R(rd) = SEXT(Mr(R(rs1) + imm, 1), 8))
INST_EXEC(lbu,    I, R(rd) = Mr(R(rs1) + imm, 1))
INST_EXEC(lh,     I, R(rd) = SEXT(Mr(R(rs1) + imm, 2), 16))
INST_EXEC(lhu,    I, R(rd) = Mr(R(rs1) + imm, 2))
INST_EXEC(lw,     I, R(rd) = Mr(R(rs1) + imm, 4))
INST_EXEC(ori,    I, R(rd) = R(rs1) | imm)
INST_EXEC(slli,   I, R(rd) = R(rs1) << shamt)
INST_EXEC(slti,   I, R(rd) = (sword_t)R(rs1) < (sword_t)imm)
INST_EXEC(sltiu,  I, R(rd) = R(rs1) < imm)
INST_EXEC(srai,   I, R(rd) = (sword_t)R(rs1) >> shamt)
INST_EXEC(srli,   I, R(rd) = R(rs1) >> shamt)
INST_EXEC(xori,   I, R(rd) = R(rs1) ^ imm)
INST_EXEC(sb,     S, Mw(R(rs1) + imm, 1, R(rs2)))
INST_EXEC(sh,     S, Mw(R(rs1) + imm, 2, R(rs2)))
INST_EXEC(sw,     S, Mw(R(rs1) + imm, 4, R(rs2)))
INST_EXEC(beq,    B, if (R(rs1) == R(rs2)) NPC = PC + imm)
INST_EXEC(bge,    B, if ((sword_t)R(rs1) >= (sword_t)R(rs2)) NPC = PC + imm)
INST_EXEC(bgeu,   B, if (R(rs1) >= R(rs2)) NPC = PC + imm)
INST_EXEC(blt,    B, if ((sword_t)R(rs1) < (sword_t)R(rs2)) NPC = PC + imm)
INST_EXEC(bltu,   B, if (R(rs1) < R(rs2)) NPC = PC + imm)
INST_EXEC(bne,    B, if (R(rs1) != R(rs2)) NPC = PC + imm)
INST_EXEC(auipc,  U, R(rd) = PC + imm)
INST_EXEC(lui,    U, R(rd) = imm)
INST_EXEC(jal,    J, R(rd) = PC + 4, NPC = PC + imm)

void init_inst_pool() {
    memset(pool, 0, sizeof(pool));

    RULE(add,    R, 0b0000000, 0b000, 0b0110011);
    RULE(and,    R, 0b0000000, 0b111, 0b0110011);
    RULE(div,    R, 0b0000001, 0b100, 0b0110011);
    RULE(divu,   R, 0b0000001, 0b101, 0b0110011);
    RULE(mul,    R, 0b0000001, 0b000, 0b0110011);
    RULE(mulh,   R, 0b0000001, 0b001, 0b0110011);
    RULE(mulhu,  R, 0b0000001, 0b011, 0b0110011);
    RULE(or,     R, 0b0000000, 0b110, 0b0110011);
    RULE(rem,    R, 0b0000001, 0b110, 0b0110011);
    RULE(remu,   R, 0b0000001, 0b111, 0b0110011);
    RULE(sll,    R, 0b0000000, 0b001, 0b0110011);
    RULE(slt,    R, 0b0000000, 0b010, 0b0110011);
    RULE(sltu,   R, 0b0000000, 0b011, 0b0110011);
    RULE(sra,    R, 0b0100000, 0b101, 0b0110011);
    RULE(srl,    R, 0b0000000, 0b101, 0b0110011);
    RULE(sub,    R, 0b0100000, 0b000, 0b0110011);
    RULE(xor,    R, 0b0000000, 0b100, 0b0110011);
    RULE(addi,   I, _________, 0b000, 0b0010011);
    RULE(andi,   I, _________, 0b111, 0b0010011);
    RULE(ebreak, I, 0b0000000, 0b000, 0b1110011);
    RULE(jalr,   I, _________, 0b000, 0b1100111);
    RULE(lb,     I, _________, 0b000, 0b0000011);
    RULE(lbu,    I, _________, 0b100, 0b0000011);
    RULE(lh,     I, _________, 0b001, 0b0000011);
    RULE(lhu,    I, _________, 0b101, 0b0000011);
    RULE(lw,     I, _________, 0b010, 0b0000011);
    RULE(ori,    I, _________, 0b110, 0b0010011);
    RULE(slli,   I, 0b000000,  0b001, 0b0010011);
    RULE(slti,   I, _________, 0b010, 0b0010011);
    RULE(sltiu,  I, _________, 0b011, 0b0010011);
    RULE(srai,   I, 0b010000,  0b101, 0b0010011);
    RULE(srli,   I, 0b000000,  0b101, 0b0010011);
    RULE(xori,   I, _________, 0b100, 0b0010011);
    RULE(sb,     S, _________, 0b000, 0b0100011);
    RULE(sh,     S, _________, 0b001, 0b0100011);
    RULE(sw,     S, _________, 0b010, 0b0100011);
    RULE(beq,    B, _________, 0b000, 0b1100011);
    RULE(bge,    B, _________, 0b101, 0b1100011);
    RULE(bgeu,   B, _________, 0b111, 0b1100011);
    RULE(blt,    B, _________, 0b100, 0b1100011);
    RULE(bltu,   B, _________, 0b110, 0b1100011);
    RULE(bne,    B, _________, 0b001, 0b1100011);
    RULE(auipc,  U, _________, _____, 0b0010111);
    RULE(lui,    U, _________, _____, 0b0110111);
    RULE(jal,    J, _________, _____, 0b1101111);
}

__attribute__((always_inline))
static inline void exec_inst() {
    if (pool[inst.opcode].funct3_pool[inst.funct3].handler) {
        pool[inst.opcode].funct3_pool[inst.funct3].handler();
    } else if (pool[inst.opcode].funct3_pool[inst.funct3].funct7_pool[inst.funct7].handler) {
        pool[inst.opcode].funct3_pool[inst.funct3].funct7_pool[inst.funct7].handler();
    } else if (pool[inst.opcode].handler) {
        pool[inst.opcode].handler();
    } else if (pool[inst.opcode].funct3_pool[inst.funct3].funct7_pool[inst.funct7 >> 1].handler) {
        pool[inst.opcode].funct3_pool[inst.funct3].funct7_pool[inst.funct7 >> 1].handler();
    } else {
        Error("Failed to exec inst 0x%" PRIx32 " at PC 0x%" PRIaddr "", inst.raw_inst, cpu.pc);
        SET_STATE(ABORT);
    }
    R(0) = 0; // reset $zero
}

__attribute__((always_inline))
void inst_exec_once(exec_t *info) {
    extern void itrace(exec_t *info);

    // Inst fetch
    inst = (inst_t)vaddr_read(info->snpc, sizeof(inst_t));

    itrace(info);
    info->snpc += (vaddr_t)sizeof(inst_t);
    _info = info;
    _info->dnpc = _info->snpc;

    // Inst exec
    exec_inst();
}
