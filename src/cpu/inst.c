#include <cpu/cpu.h>
#include <cpu/inst.h>
#include <cpu/reg.h>
#include <memory.h>
#include <utils/state.h>
#include <assert.h>

#define R(i) gpr(i)
#define CSR(i) csr(i)
#define Mr vaddr_read
#define Mw vaddr_write
#define PC info->pc
#define NPC info->dnpc

typedef enum { R_type, I_type, S_type, B_type, U_type, J_type, N_type, INVALID } inst_type;

static inline inst_type get_inst_type(exec_t *info) {
    if (info->inst.raw_inst == 0b00000000000100000000000001110011 ||
        info->inst.raw_inst == 0b00000000000000000000000001110011 ||
        info->inst.raw_inst == 0b00110000001000000000000001110011) {
        return N_type;
    }
    switch (info->inst.opcode) {
        case 0b0110011:
            return R_type;
        case 0b0000011: // lb, lh...
        case 0b0010011: // addi, slti...
        case 0b1100111: // jalr...
        case 0b0001111: // fence...
        case 0b1110011: // csrrw, csrrw...
            return I_type;
        case 0b0100011:
            return S_type;
        case 0b1100011:
            return B_type;
        case 0b0110111:
        case 0b0010111:
            return U_type;
        case 0b1101111:
            return J_type;
        default:
            Error("Unknown opcode 0x%" PRIx32 "", info->inst.opcode);
            Assert(0);
    }
    return INVALID; // won't reach here
}

static inline void exec_inst(inst_type type, exec_t *info) {
    inst_t inst = info->inst;

#define RULE_START if (0) { ; }
#define RULE_END else { Error("Match failed at PC 0x%" PRIaddr "", PC); SET_STATE(ABORT); }

#define RTYPE_RULE(NAME, _FUNCT7, _FUNCT3, _OPCODE, ...)          else if (inst.R_type.funct3 == (_FUNCT3) && inst.R_type.funct7 == (_FUNCT7)) { __VA_ARGS__ ; }
#define ITYPE_RULE(NAME, _FUNCT3, _OPCODE, ...)                   else if (inst.I_type.opcode == (_OPCODE) && inst.I_type.funct3 == (_FUNCT3)) { __VA_ARGS__ ; }
#define ITYPE_RULE_SHAMT(NAME, _INST31_26, _FUNCT3, _OPCODE, ...) else if (inst.I_type.opcode == (_OPCODE) && inst.I_type.funct3 == (_FUNCT3) && inst31_26 == (_INST31_26)) { __VA_ARGS__ ; }
#define STYPE_RULE(NAME, _FUNCT3, _OPCODE, ...)                   else if (inst.S_type.funct3 == (_FUNCT3)) { __VA_ARGS__ ; }
#define BTYPE_RULE(NAME, _FUNCT3, _OPCODE, ...)                   else if (inst.B_type.funct3 == (_FUNCT3)) { __VA_ARGS__ ; }
#define UTYPE_RULE(NAME, _OPCODE, ...)                            else if (inst.U_type.opcode == (_OPCODE)) { __VA_ARGS__ ; }
#define JTYPE_RULE(NAME, _OPCODE, ...)                            else if (inst.J_type.opcode == (_OPCODE)) { __VA_ARGS__ ; }
#define NTYPE_RULE(NAME, _RAWINST, ...)                           else if (raw_inst == (_RAWINST)) { __VA_ARGS__ ; }

#define MATCH_RTYPE() \
    do { \
        uint32_t rd __attribute__((unused)) = inst.R_type.rd, \
        rs1 __attribute__((unused)) = inst.R_type.rs1, \
        rs2 __attribute__((unused)) = inst.R_type.rs2; \
        RULE_START \
        RTYPE_RULE("add",   0b0000000, 0b000, 0b0110011, R(rd) = R(rs1) + R(rs2)) \
        RTYPE_RULE("and",   0b0000000, 0b111, 0b0110011, R(rd) = R(rs1) & R(rs2)) \
        RTYPE_RULE("div",   0b0000001, 0b100, 0b0110011, R(rd) = (sword_t)R(rs1) / (sword_t)R(rs2)) \
        RTYPE_RULE("divu",  0b0000001, 0b101, 0b0110011, R(rd) = R(rs1) / R(rs2)) \
        RTYPE_RULE("mul",   0b0000001, 0b000, 0b0110011, R(rd) = R(rs1) * R(rs2)) \
        RTYPE_RULE("mulh",  0b0000001, 0b001, 0b0110011, R(rd) = (int64_t)(sword_t)R(rs1) * (int64_t)(sword_t)R(rs2) >> 32LL) \
        RTYPE_RULE("mulhu", 0b0000001, 0b011, 0b0110011, R(rd) = (uint64_t)R(rs1) * (uint64_t)R(rs2) >> 32ULL) \
        RTYPE_RULE("or",    0b0000000, 0b110, 0b0110011, R(rd) = R(rs1) | R(rs2)) \
        RTYPE_RULE("rem",   0b0000001, 0b110, 0b0110011, R(rd) = (sword_t)R(rs1) % (sword_t)R(rs2)) \
        RTYPE_RULE("remu",  0b0000001, 0b111, 0b0110011, R(rd) = R(rs1) % R(rs2)) \
        RTYPE_RULE("sll",   0b0000000, 0b001, 0b0110011, R(rd) = R(rs1) << R(rs2)) \
        RTYPE_RULE("slt",   0b0000000, 0b010, 0b0110011, R(rd) = (sword_t)R(rs1) < (sword_t)R(rs2)) \
        RTYPE_RULE("sltu",  0b0000000, 0b011, 0b0110011, R(rd) = R(rs1) < R(rs2)) \
        RTYPE_RULE("sra",   0b0100000, 0b101, 0b0110011, R(rd) = (sword_t)R(rs1) >> (R(rs2) & 0x1f)) \
        RTYPE_RULE("srl",   0b0000000, 0b101, 0b0110011, R(rd) = R(rs1) >> (R(rs2) & 0x1f)) \
        RTYPE_RULE("sub",   0b0100000, 0b000, 0b0110011, R(rd) = R(rs1) - R(rs2)) \
        RTYPE_RULE("xor",   0b0000000, 0b100, 0b0110011, R(rd) = R(rs1) ^ R(rs2)) \
        RULE_END \
    } while (0)

#define MATCH_ITYPE() \
    do { \
        uint32_t rd __attribute__((unused)) = inst.I_type.rd, \
        rs1 __attribute__((unused)) = inst.I_type.rs1; \
        word_t imm __attribute__((unused)) = SEXT(inst.I_type.imm, 12); \
        uint32_t inst31_26 = (imm >> 6) & 0x3f; \
        uint32_t shamt __attribute__((unused)) = imm & 0x3f; \
        static word_t t; \
        RULE_START \
        ITYPE_RULE(      "addi",           0b000, 0b0010011, R(rd) = R(rs1) + imm) \
        ITYPE_RULE(      "andi",           0b111, 0b0010011, R(rd) = R(rs1) & imm) \
        ITYPE_RULE(      "csrrs",          0b010, 0b1110011, t = CSR(imm), CSR(imm) = (t | R(rs1)), R(rd) = t) \
        ITYPE_RULE(      "csrrw",          0b001, 0b1110011, t = CSR(imm), CSR(imm) = R(rs1), R(rd) = t) \
        ITYPE_RULE(      "jalr",           0b000, 0b1100111, R(rd) = PC + 4, NPC = (R(rs1) + imm) & ~1) \
        ITYPE_RULE(      "lb",             0b000, 0b0000011, R(rd) = SEXT(Mr(R(rs1) + imm, 1), 8)) \
        ITYPE_RULE(      "lbu",            0b100, 0b0000011, R(rd) = Mr(R(rs1) + imm, 1)) \
        ITYPE_RULE(      "lh",             0b001, 0b0000011, R(rd) = SEXT(Mr(R(rs1) + imm, 2), 16)) \
        ITYPE_RULE(      "lhu",            0b101, 0b0000011, R(rd) = Mr(R(rs1) + imm, 2)) \
        ITYPE_RULE(      "lw",             0b010, 0b0000011, R(rd) = Mr(R(rs1) + imm, 4)) \
        ITYPE_RULE(      "ori",            0b110, 0b0010011, R(rd) = R(rs1) | imm) \
        ITYPE_RULE(      "slti",           0b010, 0b0010011, R(rd) = (sword_t)R(rs1) < (sword_t)imm) \
        ITYPE_RULE(      "sltiu",          0b011, 0b0010011, R(rd) = R(rs1) < imm) \
        ITYPE_RULE(      "xori",           0b100, 0b0010011, R(rd) = R(rs1) ^ imm) \
        ITYPE_RULE_SHAMT("slli", 0b000000, 0b001, 0b0010011, R(rd) = R(rs1) << shamt) \
        ITYPE_RULE_SHAMT("srai", 0b010000, 0b101, 0b0010011, R(rd) = (sword_t)R(rs1) >> shamt) \
        ITYPE_RULE_SHAMT("srli", 0b000000, 0b101, 0b0010011, R(rd) = R(rs1) >> shamt) \
        RULE_END \
    } while (0)

#define MATCH_STYPE() \
    do { \
        uint32_t rs1 __attribute__((unused)) = inst.S_type.rs1, \
        rs2 __attribute__((unused)) = inst.S_type.rs2; \
        word_t imm __attribute__((unused)) = (SEXT(inst.S_type.imm11_5, 7) << 5) | inst.S_type.imm4_0; \
        RULE_START \
        STYPE_RULE("sb", 0b000, 0b0100011, Mw(R(rs1) + imm, 1, R(rs2))) \
        STYPE_RULE("sh", 0b001, 0b0100011, Mw(R(rs1) + imm, 2, R(rs2))) \
        STYPE_RULE("sw", 0b010, 0b0100011, Mw(R(rs1) + imm, 4, R(rs2))) \
        RULE_END \
    } while (0)

#define MATCH_BTYPE() \
    do { \
        uint32_t rs1 __attribute__((unused)) = inst.B_type.rs1, \
        rs2 __attribute__((unused)) = inst.B_type.rs2; \
        word_t imm __attribute__((unused)) = SEXT(inst.B_type.imm12 << 12 | inst.B_type.imm11 << 11 | inst.B_type.imm10_5 << 5 | inst.B_type.imm4_1 << 1, 13); \
        RULE_START \
        BTYPE_RULE("beq",  0b000, 0b1100011, if (R(rs1) == R(rs2)) NPC = PC + imm) \
        BTYPE_RULE("bge",  0b101, 0b1100011, if ((sword_t)R(rs1) >= (sword_t)R(rs2)) NPC = PC + imm) \
        BTYPE_RULE("bgeu", 0b111, 0b1100011, if (R(rs1) >= R(rs2)) NPC = PC + imm) \
        BTYPE_RULE("blt",  0b100, 0b1100011, if ((sword_t)R(rs1) < (sword_t)R(rs2)) NPC = PC + imm) \
        BTYPE_RULE("bltu", 0b110, 0b1100011, if (R(rs1) < R(rs2)) NPC = PC + imm) \
        BTYPE_RULE("bne",  0b001, 0b1100011, if (R(rs1) != R(rs2)) NPC = PC + imm) \
        RULE_END \
    } while (0)

#define MATCH_UTYPE() \
    do { \
        uint32_t rd __attribute__((unused)) = inst.U_type.rd; \
        word_t imm __attribute__((unused)) = SEXT(inst.U_type.imm31_12, 20) << 12; \
        RULE_START \
        UTYPE_RULE("auipc", 0b0010111, R(rd) = PC + imm) \
        UTYPE_RULE("lui",   0b0110111, R(rd) = imm) \
        RULE_END \
    } while (0)

#define MATCH_JTYPE() \
    do { \
        uint32_t rd __attribute__((unused)) = inst.J_type.rd; \
        word_t imm __attribute__((unused)) = SEXT(inst.J_type.imm20 << 20 | inst.J_type.imm19_12 << 12 | inst.J_type.imm11 << 11 | inst.J_type.imm10_1 << 1, 21); \
        RULE_START \
        JTYPE_RULE("jal", 0b1101111, R(rd) = PC + 4, NPC = PC + imm) \
        RULE_END \
    } while (0)

#define MATCH_NTYPE() \
    do { \
        uint32_t raw_inst = inst.raw_inst; \
        RULE_START \
        NTYPE_RULE("mret",   0b00110000001000000000000001110011, NPC = CSR(mepc); WRITE_CSR(mstatus, MIE, READ_CSR(mstatus, MPIE)); WRITE_CSR(mstatus, MPIE, 1)) \
        NTYPE_RULE("ecall",  0b00000000000000000000000001110011, NPC = RAISE_INTR(0xb, PC)) \
        NTYPE_RULE("ebreak", 0b00000000000100000000000001110011, SET_STATE(END)) \
        RULE_END \
    } while (0)

    switch (type) {
        case R_type: MATCH_RTYPE(); break;
        case I_type: MATCH_ITYPE(); break;
        case S_type: MATCH_STYPE(); break;
        case B_type: MATCH_BTYPE(); break;
        case U_type: MATCH_UTYPE(); break;
        case J_type: MATCH_JTYPE(); break;
        case N_type: MATCH_NTYPE(); break;
        default: Assert(0);
    }

    R(0) = 0; // reset $zero
}

static void inst_decode(exec_t *info) {
    info->dnpc = info->snpc;
    inst_type type = get_inst_type(info);
    exec_inst(type, info);
}

void inst_exec_once(exec_t *info) {
    extern void itrace(exec_t *info);
    // Inst fetch
    info->inst = (inst_t)vaddr_fetchinst(info->snpc, sizeof(inst_t));
    itrace(info);
    info->snpc += (vaddr_t)sizeof(inst_t);
    // Inst decode and exec
    inst_decode(info);
}
