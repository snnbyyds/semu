#include <cpu/cpu.h>
#include <cpu/inst.h>
#include <cpu/reg.h>
#include <memory.h>
#include <utils/state.h>
#include <assert.h>

#define R(i) gpr(i)
#define Mr vaddr_read
#define Mw vaddr_write
#define PC info->pc
#define NPC info->dnpc

typedef enum { R_type, I_type, S_type, B_type, U_type, J_type, N_type, INVALID } inst_type;

static inline inst_type get_inst_type(exec_t *info) {
    switch (info->inst.opcode) {
        case 0b0110011:
            return R_type;
        case 0b0000011: // lb, lh...
        case 0b0010011: // addi, slti...
        case 0b1100111: // jalr...
        case 0b0001111: // fence...
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
        case 0b1110011:
            return N_type; // A special type that can be matched directly with raw inst
        default:
            printf("Unknown opcode 0x%" PRIx32 "\n", info->inst.opcode);
            assert(0);
    }
    return INVALID; // won't reach here
}

static inline void exec_inst(inst_type type, exec_t *info) {
    inst_t inst = info->inst;

#define RULE_START if (0) { ; }
#define RULE_END else { Warn("Match failed"); SET_STATE(ABORT); }

#define RTYPE_RULE(NAME, _FUNCT7, _FUNCT3, _OPCODE, ...) else if (opcode == (_OPCODE) && funct3 == (_FUNCT3) && funct7 == (_FUNCT7)) { __VA_ARGS__ ; }
#define ITYPE_RULE(NAME, _FUNCT3, _OPCODE, ...)          else if (opcode == (_OPCODE) && funct3 == (_FUNCT3)) { __VA_ARGS__ ; }
#define STYPE_RULE(NAME, _FUNCT3, _OPCODE, ...)          else if (opcode == (_OPCODE) && funct3 == (_FUNCT3)) { __VA_ARGS__ ; }
#define BTYPE_RULE(NAME, _FUNCT3, _OPCODE, ...)          else if (opcode == (_OPCODE) && funct3 == (_FUNCT3)) { __VA_ARGS__ ;}
#define UTYPE_RULE(NAME, _OPCODE, ...)                   else if (opcode == (_OPCODE)) { __VA_ARGS__ ; }
#define JTYPE_RULE(NAME, _OPCODE, ...)                   else if (opcode == (_OPCODE)) { __VA_ARGS__ ; }
#define NTYPE_RULE(NAME, _RAWINST, ...)                  else if (raw_inst == (_RAWINST)) { __VA_ARGS__ ; }

#define MATCH_RTYPE() \
    do { \
        uint32_t opcode = inst.R_type.opcode, \
        funct3 = inst.R_type.funct3, \
        funct7 = inst.R_type.funct7, \
        rd __attribute__((unused)) = inst.R_type.rd, \
        rs1 __attribute__((unused)) = inst.R_type.rs1, \
        rs2 __attribute__((unused)) = inst.R_type.rs2; \
        RULE_START \
        RTYPE_RULE("add", 0b0000000, 0b000, 0b0110011, R(rd) = R(rs1) + R(rs2)) \
        RULE_END \
    } while (0)

#define MATCH_ITYPE() \
    do { \
        uint32_t opcode = inst.I_type.opcode, \
        funct3 = inst.I_type.funct3, \
        rd __attribute__((unused)) = inst.I_type.rd, \
        rs1 __attribute__((unused)) = inst.I_type.rs1; \
        word_t imm __attribute__((unused)) = SEXT(inst.I_type.imm, 12); \
        RULE_START \
        ITYPE_RULE("lbu", 0b100, 0b0000011, R(rd) = Mr(R(rs1) + imm, 1)) \
        RULE_END \
    } while (0)

#define MATCH_STYPE() \
    do { \
        uint32_t opcode = inst.S_type.opcode, \
        funct3 = inst.S_type.funct3, \
        rs1 __attribute__((unused)) = inst.S_type.rs1, \
        rs2 __attribute__((unused)) = inst.S_type.rs2; \
        word_t imm __attribute__((unused)) = (SEXT(inst.S_type.imm11_5, 7) << 5) | inst.S_type.imm4_0; \
        RULE_START \
        STYPE_RULE("sb", 0b000, 0b0100011, Mw(R(rs1) + imm, 1, R(rs2))) \
        RULE_END \
    } while (0)

#define MATCH_BTYPE() \
    do { \
        uint32_t opcode = inst.B_type.opcode, \
        funct3 = inst.B_type.funct3, \
        rs1 __attribute__((unused)) = inst.B_type.rs1, \
        rs2 __attribute__((unused)) = inst.B_type.rs2; \
        word_t imm __attribute__((unused)) = SEXT(inst.B_type.imm12 << 12 | inst.B_type.imm11 << 11 | inst.B_type.imm10_5 << 5 | inst.B_type.imm4_1 << 1, 13); \
        RULE_START \
        BTYPE_RULE("beq", 0b000, 0b1100011, if (R(rs1) == R(rs2)) NPC = PC + imm) \
        RULE_END \
    } while (0)

#define MATCH_UTYPE() \
    do { \
        uint32_t opcode = inst.U_type.opcode, \
        rd __attribute__((unused)) = inst.U_type.rd; \
        word_t imm __attribute__((unused)) = SEXT(inst.U_type.imm31_12, 20) << 12; \
        RULE_START \
        UTYPE_RULE("auipc", 0b0010111, R(rd) = PC + imm) \
        RULE_END \
    } while (0)

#define MATCH_JTYPE() \
    do { \
        uint32_t opcode = inst.J_type.opcode, \
        rd __attribute__((unused)) = inst.J_type.rd; \
        word_t imm __attribute__((unused)) = SEXT(inst.J_type.imm20 << 20 | inst.J_type.imm19_12 << 12 | inst.J_type.imm11 << 11 | inst.J_type.imm10_1 << 1, 21); \
        RULE_START \
        JTYPE_RULE("jal", 0b1101111, R(rd) = PC + 4, NPC = PC + imm) \
        RULE_END \
    } while (0)

#define MATCH_NTYPE() \
    do { \
        uint32_t raw_inst = inst.raw_inst; \
        RULE_START \
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
        default: assert(0);
    }

    R(0) = 0; // reset $zero
}

static void inst_decode(exec_t *info) {
    info->dnpc = info->snpc;
    inst_type type = get_inst_type(info);
    exec_inst(type, info);
}

void inst_exec_once(exec_t *info) {
    // Inst fetch
    info->inst = (inst_t)vaddr_read(info->snpc, sizeof(inst_t));
    printf("exec: 0x%" PRIx32 "\n", *(uint32_t *)&info->inst);
    info->snpc += (vaddr_t)sizeof(inst_t);
    // Inst decode and exec
    inst_decode(info);
}
