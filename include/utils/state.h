#ifndef __STATE_H__
#define __STATE_H__

#include <cpu/cpu.h>
#include <cpu/reg.h>

typedef enum { RUNNING, STOP, END, QUIT, ABORT } state_t;

typedef struct {
    state_t state;
    vaddr_t halt_pc;
    word_t halt_code;
} EMU_State;

bool is_good_exit_state();

extern EMU_State semu_state;

#define SET_STATE(S) ({ semu_state.state = (S); semu_state.halt_pc = cpu.pc; semu_state.halt_code = gpr(10); })

#endif
