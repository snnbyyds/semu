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

#ifndef __FPU_H__
#define __FPU_H__

#include "reg.h"

/* REF: https://github.com/sysprog21/rv32emu/blob/c62d2317730c77b3c5e1258bbbe25df02b409145/src/softfloat.h */

/* S | Exponent (8 bits) | Mantissa (23 bits) */
#define FLOAT_SIGN_MASK 0x80000000
#define FLOAT_EXPN_MASK 0x7f800000
#define FLOAT_FRAC_MASK 0x007fffff
#define FLOAT_QNAN_MASK 0x00400000

/* Classify a float
 * @f: 32-bit IEEE 754 floating-point number
 * @return: classification code (bit pattern) of the float
 *
 * Classification codes:
 * 0x001: Negative infinity
 * 0x002: Negative normal number
 * 0x004: Negative subnormal number
 * 0x008: Negative zero
 * 0x010: Positive zero
 * 0x020: Positive subnormal number
 * 0x040: Positive normal number
 * 0x080: Positive infinity
 * 0x100: Signaling NaN
 * 0x200: Quiet NaN
 */
__attribute__((always_inline))
static inline uint32_t classify_s(uint32_t f) {
    const uint32_t sign = f & FLOAT_SIGN_MASK;
    const uint32_t expn = f & FLOAT_EXPN_MASK;
    const uint32_t frac = f & FLOAT_FRAC_MASK;

    if (expn) {
        if (expn != FLOAT_EXPN_MASK) {
            return sign ? 0x002 : 0x040; // Negative or positive normal number
        } else {
            if (frac) {
                return frac & FLOAT_QNAN_MASK ? 0x200 : 0x100; // Quiet NaN or Signaling NaN
            } else if (!sign) {
                return 0x080; // Positive infinity
            } else {
                return 0x001; // Negative infinity
            }
        }
    } else if (frac) {
        return sign ? 0x004 : 0x020; // Negative or positive subnormal number
    } else {
        return sign ? 0x008 : 0x010; // Negative or positive zero
    }

    __builtin_unreachable();
    return -1;
}

/* Check if a 32-bit float is NaN
 * @f: 32-bit IEEE 754 floating-point number
 * @return: true if NaN, false otherwise
 */
__attribute__((always_inline))
static inline bool is_nan(uint32_t f) {
    return (f & FLOAT_EXPN_MASK) == FLOAT_EXPN_MASK && (f & FLOAT_FRAC_MASK);
}

/* D | Sign (1 bit) | Exponent (11 bits) | Mantissa (52 bits) */
#define DOUBLE_SIGN_MASK 0x8000000000000000ULL
#define DOUBLE_EXPN_MASK 0x7FF0000000000000ULL
#define DOUBLE_FRAC_MASK 0x000FFFFFFFFFFFFFULL
#define DOUBLE_QNAN_MASK 0x0008000000000000ULL /* Quiet NaN (most significant bit of mantissa) */

/* Classify a double
 * @d: 64-bit IEEE 754 floating-point number
 * @return: classification code (bit pattern) of the double
 *
 * Classification codes:
 * 0x001: Negative infinity
 * 0x002: Negative normal number
 * 0x004: Negative subnormal number
 * 0x008: Negative zero
 * 0x010: Positive zero
 * 0x020: Positive subnormal number
 * 0x040: Positive normal number
 * 0x080: Positive infinity
 * 0x100: Signaling NaN
 * 0x200: Quiet NaN
 */
__attribute__((always_inline))
static inline uint32_t classify_d(uint64_t d) {
    const uint64_t sign = d & DOUBLE_SIGN_MASK;
    const uint64_t expn = d & DOUBLE_EXPN_MASK;
    const uint64_t frac = d & DOUBLE_FRAC_MASK;

    if (expn) {
        if (expn != DOUBLE_EXPN_MASK) {
            return sign ? 0x002 : 0x040; // Negative or positive normal number
        } else {
            if (frac) {
                return frac & DOUBLE_QNAN_MASK ? 0x200 : 0x100; // Quiet NaN or Signaling NaN
            } else if (!sign) {
                return 0x080; // Positive infinity
            } else {
                return 0x001; // Negative infinity
            }
        }
    } else if (frac) {
        return sign ? 0x004 : 0x020; // Negative or positive subnormal number
    } else {
        return sign ? 0x008 : 0x010; // Negative or positive zero
    }

    __builtin_unreachable();
    return -1;
}

/* Check if a 64-bit float is NaN
 * @f: 64-bit IEEE 754 floating-point number
 * @return: true if NaN, false otherwise
 */
__attribute__((always_inline))
static inline bool is_nan64(uint64_t f) {
    return ((~f & DOUBLE_EXPN_MASK) == 0) && (f & DOUBLE_FRAC_MASK);
}

/* Set rounding mode */
__attribute__((always_inline))
static inline void set_rounding_mode(uint32_t rm) {
    if (likely(rm == 0b111)) {
        rm = (cpu.csr_fcsr & FCSR_FRM_MASK) >> FCSR_FRM_SHIFT;
    }

    switch (rm) {
        case 0b000: /* RNE */
            softfloat_roundingMode = softfloat_round_near_even;
            break;
        case 0b001: /* RTZ */
            softfloat_roundingMode = softfloat_round_minMag;
            break;
        case 0b010: /* RDN */
            softfloat_roundingMode = softfloat_round_min;
            break;
        case 0b011: /* RUP */
            softfloat_roundingMode = softfloat_round_max;
            break;
        case 0b100: /* RMM */
            softfloat_roundingMode = softfloat_round_near_maxMag;
            break;
        case 0b101: /* Reserved */
        case 0b110: /* Reserved */
        case 0b111: /* DYN */
        default:
            __builtin_unreachable();
            break;
    }
}

/* Set fflag */
__attribute__((always_inline))
static inline void set_fflag() {
    uint32_t flag = 0;
    if (softfloat_exceptionFlags & softfloat_flag_inexact) {
        flag |= FCSR_NX_MASK;
    }
    if (softfloat_exceptionFlags & softfloat_flag_underflow) {
        flag |= FCSR_UF_MASK;
    }
    if (softfloat_exceptionFlags & softfloat_flag_overflow) {
        flag |= FCSR_OF_MASK;
    }
    if (softfloat_exceptionFlags & softfloat_flag_infinite) {
        flag |= FCSR_DZ_MASK;
    }
    if (softfloat_exceptionFlags & softfloat_flag_invalid) {
        flag |= FCSR_NV_MASK;
    }
    cpu.csr_fcsr |= flag;
    softfloat_exceptionFlags = 0;
}

/* Accesses a 32-bit single-precision floating-point
 * value from the floating-point register at index `idx`.
 */
#define ACCESS_F32(idx) (cpu.fpr[idx]._32)

/* Accesses a 64-bit single-precision floating-point
 * value from the floating-point register at index `idx`.
 */
#define ACCESS_F64(idx) (cpu.fpr[idx]._64)

#define SET_FPR_NAN_BOX_UPPER_BITS(idx) \
    ({ \
        cpu.fpr[idx].v64 |= UINT64_C(0xFFFFFFFF00000000); \
    })

__attribute__((always_inline))
static inline float32_t unboxF32(fpr_t src) {
    if (likely(src.v64 & UINT64_C(0xFFFFFFFF00000000)) == UINT64_C(0xFFFFFFFF00000000)) {
        return src._32;
    }
    return (float32_t) { .v = 0x7fc00000 };
}

#endif
