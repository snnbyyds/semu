#ifndef __REG_H__
#define __REG_H__

#include "cpu.h"

#define gpr(i) cpu.gpr[i]

// MSTATUS field masks and shifts
#define MSTATUS_UIE_SHIFT 0
#define MSTATUS_SIE_SHIFT 1
#define MSTATUS_WPRI_SHIFT 2
#define MSTATUS_MIE_SHIFT 3
#define MSTATUS_UPIE_SHIFT 4
#define MSTATUS_SPIE_SHIFT 5
#define MSTATUS_WPRI2_SHIFT 6
#define MSTATUS_MPIE_SHIFT 7
#define MSTATUS_SPP_SHIFT 8
#define MSTATUS_WPRI3_SHIFT 9
#define MSTATUS_MPP_SHIFT 11
#define MSTATUS_FS_SHIFT 13
#define MSTATUS_XS_SHIFT 15
#define MSTATUS_MPRV_SHIFT 17
#define MSTATUS_SUM_SHIFT 18
#define MSTATUS_MXR_SHIFT 19
#define MSTATUS_TVM_SHIFT 20
#define MSTATUS_TW_SHIFT 21
#define MSTATUS_TSR_SHIFT 22
#define MSTATUS_WPRI4_SHIFT 23
#define MSTATUS_SD_SHIFT 31
#define MSTATUS_UIE (1 << MSTATUS_UIE_SHIFT)
#define MSTATUS_SIE (1 << MSTATUS_SIE_SHIFT)
#define MSTATUS_WPRI (1 << MSTATUS_WPRI_SHIFT)
#define MSTATUS_MIE (1 << MSTATUS_MIE_SHIFT)
#define MSTATUS_UPIE (1 << MSTATUS_UPIE_SHIFT)
#define MSTATUS_SPIE (1 << MSTATUS_SPIE_SHIFT)
#define MSTATUS_WPRI2 (1 << MSTATUS_WPRI2_SHIFT)
#define MSTATUS_MPIE (1 << MSTATUS_MPIE_SHIFT)
#define MSTATUS_SPP (1 << MSTATUS_SPP_SHIFT)
#define MSTATUS_WPRI3 (3 << MSTATUS_WPRI3_SHIFT) // 2 bits
#define MSTATUS_MPP (3 << MSTATUS_MPP_SHIFT)     // 2 bits
#define MSTATUS_FS (3 << MSTATUS_FS_SHIFT)       // 2 bits
#define MSTATUS_XS (3 << MSTATUS_XS_SHIFT)       // 2 bits
#define MSTATUS_MPRV (1 << MSTATUS_MPRV_SHIFT)
#define MSTATUS_SUM (1 << MSTATUS_SUM_SHIFT)
#define MSTATUS_MXR (1 << MSTATUS_MXR_SHIFT)
#define MSTATUS_TVM (1 << MSTATUS_TVM_SHIFT)
#define MSTATUS_TW (1 << MSTATUS_TW_SHIFT)
#define MSTATUS_TSR (1 << MSTATUS_TSR_SHIFT)
#define MSTATUS_WPRI4 (0xFF << MSTATUS_WPRI4_SHIFT) // 8 bits
#define MSTATUS_SD (1 << MSTATUS_SD_SHIFT)

// SATP field masks and shifts
#define SATP_PPN_SHIFT 0
#define SATP_ASID_SHIFT 22
#define SATP_MODE_SHIFT 31
#define SATP_PPN (0x3FFFFF << SATP_PPN_SHIFT) // 22 bits
#define SATP_ASID (0x1FF << SATP_ASID_SHIFT)  // 9 bits
#define SATP_MODE (1 << SATP_MODE_SHIFT)      // 1 bit

// MTVEC field masks and shifts
#define MTVEC_VALUE_SHIFT 0
#define MTVEC_VALUE (0xFFFFFFFF << MTVEC_VALUE_SHIFT) // 32 bits

// MEPC field masks and shifts
#define MEPC_VALUE_SHIFT 0
#define MEPC_VALUE (0xFFFFFFFF << MEPC_VALUE_SHIFT) // 32 bits

// MCAUSE field masks and shifts
#define MCAUSE_EXCEPTION_SHIFT 0
#define MCAUSE_INTERRUPT_SHIFT 31
#define MCAUSE_EXCEPTION (0x7FFFFFFF << MCAUSE_EXCEPTION_SHIFT) // 31 bits
#define MCAUSE_INTERRUPT (1 << MCAUSE_INTERRUPT_SHIFT)          // 1 bit

typedef enum {
    SATP     = 0x180,
    MSTATUS  = 0x300,
    MTVEC    = 0x305,
    MSCRATCH = 0x340,
    MEPC     = 0x341,
    MCAUSE   = 0x342
} CSR_Number;

static inline word_t *csr_get_ptr(CSR_Number number) {
    switch (number) {
        case SATP: // 0x180
            return &cpu.csr_satp;
        case MSTATUS: // 0x300
            return &cpu.csr_mstatus;
        case MTVEC: // 0x305
            return &cpu.csr_mtvec;
        case MSCRATCH: // 0x340
            return &cpu.csr_mscratch;
        case MEPC: // 0X341
            return &cpu.csr_mepc;
        case MCAUSE: // 0x342
            return &cpu.csr_mcause;
        default:
            assert(0);
    }
    return NULL;
}

void reg_val_display();
char *reg_val_to_name(size_t idx);

#endif
