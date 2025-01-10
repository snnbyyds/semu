#ifndef __RV32A_H__
#define __RV32A_H__

#include <cpu/fpu.h>
#include "macros.h"

extern decode_t unimpl;

/* R-type */
static inline void op_amo(uint32_t inst, exec_t *info) {
    const uint32_t funct5 = (decode_funct7(inst) >> 2) & 0x1f;
    uint32_t t;
    const uint32_t __rs1 = rs1;
    const uint32_t __rs2 = rs2;
    const uint32_t __rd = rd;
    switch (funct5) {
        case 0b00010: /* lr.w */
            R(__rd) = Mr_w(R(__rs1));
            break;
        case 0b00011: /* sc.w */
            Mw_w(R(__rs1), R(__rs2));
            R(__rd) = 0;
            break;
        case 0b00001: /* amoswap.w */
            t = Mr_w(R(__rs1));
            Mw_w(R(__rs1), R(__rs2));
            R(__rd) = t;            
            break;
        case 0b00000: /* amoadd.w */
            t = Mr_w(R(__rs1));
            Mw_w(R(__rs1), t + R(__rs2));
            R(__rd) = t;
            break;
        case 0b00100: /* amoxor.w */
            t = Mr_w(R(__rs1));
            Mw_w(R(__rs1), t ^ R(__rs2));
            R(__rd) = t;
            break;
        case 0b01100: /* amoand.w */
            t = Mr_w(R(__rs1));
            Mw_w(R(__rs1), t & R(__rs2));
            R(__rd) = t;
            break;
        case 0b01000: /* amoor.w */
            t = Mr_w(R(__rs1));
            Mw_w(R(__rs1), t | R(__rs2));
            R(__rd) = t;
            break;
        case 0b10000: /* amomin.w */
            t = Mr_w(R(__rs1));
            Mw_w(R(__rs1), (int32_t)t < (int32_t)R(__rs2) ? t : R(__rs2));
            R(__rd) = t;
            break;
        case 0b10100: /* amomax.w */
            t = Mr_w(R(__rs1));
            Mw_w(R(__rs1), (int32_t)t > (int32_t)R(__rs2) ? t : R(__rs2));
            R(__rd) = t;
            break;
        case 0b11000: /* amominu.w */
            t = Mr_w(R(__rs1));
            Mw_w(R(__rs1), t < R(__rs2) ? t : R(__rs2));
            R(__rd) = t;
            break;
        case 0b11100: /* amomaxu.w */
            t = Mr_w(R(__rs1));
            Mw_w(R(__rs1), t > R(__rs2) ? t : R(__rs2));
            R(__rd) = t;
            break;
        default:
            unimpl(inst, info);
            break;
    }
}

#endif
