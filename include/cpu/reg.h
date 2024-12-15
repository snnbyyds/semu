#ifndef __REG_H__
#define __REG_H__

#include <common.h>

#define gpr(i) cpu.gpr[i]
#define csr(i) cpu.csr[i]

typedef enum {
    satp     = 0x180,
    mstatus  = 0x300,
    mtvec    = 0x305,
    mscratch = 0x340,
    mepc     = 0x341,
    mcause   = 0x342
} CSR_Number;

typedef struct {
    const char *name;
    CSR_Number number;
    const bool implemented;
} CSR_Property;

typedef union __attribute__((packed)) {
  struct { word_t PPN : 22, ASID : 9, MODE : 1; } dec_SATP;
  struct { word_t UIE: 1, SIE: 1, WPRI: 1, MIE: 1, UPIE: 1, SPIE: 1, WPRI2: 1, MPIE: 1,
                  SPP: 1, WPRI3: 2, MPP: 2, FS: 2, XS: 2, MPRV: 1, SUM: 1, MXR: 1, TVM: 1,
                  TW: 1, TSR: 1, WPRI4: 8, SD: 1; } dec_MSTATUS;
  struct { word_t VALUE : 32; } dec_MTVEC;
  struct { word_t VALUE : 32; } dec_MSCRATCH;
  struct { word_t VALUE : 32; } dec_MEPC;
  struct { word_t EXCEPTION : 31, INTERRUPT : 1; } dec_MCAUSE;
  word_t val;
} SR_Decode;

void reg_val_display();

#endif
