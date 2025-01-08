#include <cpu/cpu.h>
#include <cpu/inst.h>
#include <cpu/reg.h>
#include <memory.h>
#include <utils/state.h>
#include <string.h>
#include <system.h>
#include <assert.h>

#define R(i) gpr(i)

#define Mr_w vaddr_read_w
#define Mr_s vaddr_read_s
#define Mr_b vaddr_read_b
#define Mw_w vaddr_write_w
#define Mw_s vaddr_write_s
#define Mw_b vaddr_write_b

#define PC _info->pc
#define NPC _info->dnpc

#define rd decode_rd(inst)
#define rs1 decode_rs1(inst)
#define rs2 decode_rs2(inst)

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
static uint32_t inst;
static exec_t *_info = NULL;

#define IMM(TYPE) decode_imm ## TYPE(inst)

#define INST_EXEC(NAME, TYPE, ...) \
    static void __ ## NAME() { \
        __VA_ARGS__; \
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
    switch (inst) {
        case 0b00000000000100000000000001110011: // ebreak
            SET_STATE(gpr(10) ? ABORT : END);
            break;
        case 0b00000000000000000000000001110011: // ecall
            NPC = ISA_RAISE_INTR(0xb, PC);
            break;
        case 0b00110000001000000000000001110011: // mret
            NPC = cpu.csr_mepc;
            const uint32_t mstatus_mpie =
                (cpu.csr_mstatus & MSTATUS_MPIE) >> MSTATUS_MPIE_SHIFT;
            cpu.csr_mstatus |= (mstatus_mpie << MSTATUS_MIE_SHIFT);
            cpu.csr_mstatus |= MSTATUS_MPIE;
            break;
        default:
            assert(0);
    }
}

INST_EXEC(add,    R, R(rd) = R(rs1) + R(rs2))
INST_EXEC(and,    R, R(rd) = R(rs1) & R(rs2))
INST_EXEC(div,    R, R(rd) = (sword_t)R(rs1) / (sword_t)R(rs2))
INST_EXEC(divu,   R, R(rd) = R(rs1) / R(rs2))
INST_EXEC(mret,   R, __special_handler())
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
INST_EXEC(addi,   I, R(rd) = R(rs1) + IMM(I))
INST_EXEC(andi,   I, R(rd) = R(rs1) & IMM(I))
INST_EXEC(csrrs,  I, word_t imm = IMM(I); word_t *c = csr_get_ptr(imm); word_t t = *c; *c = t | R(rs1); R(rd) = t)
INST_EXEC(csrrw,  I, word_t imm = IMM(I); word_t *c = csr_get_ptr(imm); word_t t = *c; *c = R(rs1); R(rd) = t)
INST_EXEC(ebreak, I, __special_handler())
INST_EXEC(ecall,  I, __special_handler())
INST_EXEC(jalr,   I, word_t t = PC + 4; NPC = (R(rs1) + IMM(I)) & ~1; R(rd) = t)
INST_EXEC(lb,     I, R(rd) = (uint32_t)((int32_t)((int8_t)(Mr_b(R(rs1) + IMM(I))))))
INST_EXEC(lbu,    I, R(rd) = Mr_b(R(rs1) + IMM(I)))
INST_EXEC(lh,     I, R(rd) = (uint32_t)((int32_t)((int16_t)(Mr_s(R(rs1) + IMM(I))))))
INST_EXEC(lhu,    I, R(rd) = Mr_s(R(rs1) + IMM(I)))
INST_EXEC(lw,     I, R(rd) = Mr_w(R(rs1) + IMM(I)))
INST_EXEC(ori,    I, R(rd) = R(rs1) | IMM(I))
INST_EXEC(slli,   I, R(rd) = R(rs1) << (IMM(I) & 0x3f))
INST_EXEC(slti,   I, R(rd) = (sword_t)R(rs1) < (sword_t)IMM(I))
INST_EXEC(sltiu,  I, R(rd) = R(rs1) < IMM(I))
INST_EXEC(srai,   I, R(rd) = (sword_t)R(rs1) >> (IMM(I) & 0x3f))
INST_EXEC(srli,   I, R(rd) = R(rs1) >> (IMM(I) & 0x3f))
INST_EXEC(xori,   I, R(rd) = R(rs1) ^ IMM(I))
INST_EXEC(sb,     S, Mw_b(R(rs1) + IMM(S), R(rs2)))
INST_EXEC(sh,     S, Mw_s(R(rs1) + IMM(S), R(rs2)))
INST_EXEC(sw,     S, Mw_w(R(rs1) + IMM(S), R(rs2)))
INST_EXEC(beq,    B, if (R(rs1) == R(rs2)) NPC = PC + IMM(B))
INST_EXEC(bge,    B, if ((sword_t)R(rs1) >= (sword_t)R(rs2)) NPC = PC + IMM(B))
INST_EXEC(bgeu,   B, if (R(rs1) >= R(rs2)) NPC = PC + IMM(B))
INST_EXEC(blt,    B, if ((sword_t)R(rs1) < (sword_t)R(rs2)) NPC = PC + IMM(B))
INST_EXEC(bltu,   B, if (R(rs1) < R(rs2)) NPC = PC + IMM(B))
INST_EXEC(bne,    B, if (R(rs1) != R(rs2)) NPC = PC + IMM(B))
INST_EXEC(auipc,  U, R(rd) = PC + IMM(U))
INST_EXEC(lui,    U, R(rd) = IMM(U))
INST_EXEC(jal,    J, R(rd) = PC + 4, NPC = PC + IMM(J))

void init_inst_pool() {
    memset(pool, 0, sizeof(pool));

    RULE(add,    R, 0b0000000, 0b000, 0b0110011);
    RULE(and,    R, 0b0000000, 0b111, 0b0110011);
    RULE(div,    R, 0b0000001, 0b100, 0b0110011);
    RULE(divu,   R, 0b0000001, 0b101, 0b0110011);
    RULE(mret,   R, 0b0011000, 0b000, 0b1110011);
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
    RULE(csrrs,  I, _________, 0b010, 0b1110011);
    RULE(csrrw,  I, _________, 0b001, 0b1110011);
    RULE(ebreak, I, 0b0000000, 0b000, 0b1110011);
    RULE(ecall,  I, 0b0000000, 0b000, 0b1110011);
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
    void (*handler)() = NULL;
    uint32_t opcode = decode_opcode(inst);
    uint32_t funct3 = decode_funct3(inst);
    uint32_t funct7 = decode_funct7(inst);
    if ((handler = pool[opcode].funct3_pool[funct3].handler)) {
        handler();
    } else if ((handler = pool[opcode].funct3_pool[funct3].funct7_pool[funct7].handler)) {
        handler();
    } else if ((handler = pool[opcode].handler)) {
        handler();
    } else if ((handler = pool[opcode].funct3_pool[funct3].funct7_pool[funct7 >> 1].handler)) {
        handler();
    } else {
        Error("Failed to exec inst 0x%" PRIx32 " at PC 0x%" PRIaddr "", inst, cpu.pc);
        SET_STATE(ABORT);
    }
    R(0) = 0; // reset $zero
}

void inst_exec_once(exec_t *info) {
    extern void itrace(exec_t *info, uint32_t inst);

    // Inst fetch
    inst = vaddr_ifetch(info->snpc);

    itrace(info, inst);
    info->snpc += (vaddr_t)sizeof(uint32_t);
    _info = info;
    _info->dnpc = _info->snpc;

    // Inst exec
    exec_inst();
}
