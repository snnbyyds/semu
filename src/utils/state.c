#include <utils/state.h>

EMU_State semu_state;

bool is_good_exit_state() {
    return (semu_state.state == END && semu_state.halt_code == 0) || semu_state.state == QUIT || semu_state.state == STOP;
}
