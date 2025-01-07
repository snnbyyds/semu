#include <cpu/cpu.h>
#include <gdbstub.h>
#include <memory.h>
#include <utils/state.h>
#include <errno.h>
#include <string.h>

#define MEM_SIZE CONFIG_MSIZE

typedef struct {
    bool bp_is_set;
    vaddr_t bp_addr;
    gdbstub_t gdbstub;
} dbg_stat_t;

static int emu_read_reg(void *args, int regno, size_t *reg_value);
static int emu_write_reg(void *args, int regno, size_t data);
static int emu_read_mem(void *args, size_t addr, size_t len, void *val);
static int emu_write_mem(void *args, size_t addr, size_t len, void *val);
static gdb_action_t emu_cont(void *args);
static gdb_action_t emu_stepi(void *args);
static bool emu_set_bp(void *args, size_t addr, bp_type_t type);
static bool emu_del_bp(void *args, size_t addr, bp_type_t type);
static void emu_on_interrupt(void *args);

static arch_info_t semu_arch_info = {
    .target_desc = TARGET_RV32,
    .reg_num = LENGTH(cpu.gpr) + 1, // gprs + pc
    .reg_byte = sizeof(cpu.gpr[0])
};

static dbg_stat_t semu_dbg_stat = {};

static struct target_ops semu_ops = {
    .read_reg = emu_read_reg,
    .write_reg = emu_write_reg,
    .read_mem = emu_read_mem,
    .write_mem = emu_write_mem,
    .cont = emu_cont,
    .stepi = emu_stepi,
    .set_bp = emu_set_bp,
    .del_bp = emu_del_bp,
    .on_interrupt = emu_on_interrupt,
};

static inline void emu_stop() {
    __atomic_store_n(&semu_state.state, STOP, __ATOMIC_RELAXED);
}

static inline bool emu_is_exited() {
    state_t state = __atomic_load_n(&semu_state.state, __ATOMIC_RELAXED);
    return state == QUIT || state == END || state == ABORT;
}

static int emu_read_reg(void *args, int regno, size_t *reg_value) {
    if (regno > semu_arch_info.reg_num - 1) {
        return EFAULT;
    }
    *reg_value = regno == semu_arch_info.reg_num - 1 ? cpu.pc : cpu.gpr[regno];
    return 0;
}

static int emu_write_reg(void *args, int regno, size_t data) {
    if (regno > semu_arch_info.reg_num - 1) {
        return EFAULT;
    }
    if (regno == semu_arch_info.reg_num - 1) {
        cpu.pc = data;
    } else {
        cpu.gpr[regno] = data;
    }
    return 0;
}

static int emu_read_mem(void *args, size_t addr, size_t len, void *val) {
    if (!IN_PMEM(addr) || !IN_PMEM(addr + len)) {
        return EFAULT;
    }
    memcpy(val, GUEST_TO_HOST(addr), len);
    return 0;
}

static int emu_write_mem(void *args, size_t addr, size_t len, void *val) {
    if (!IN_PMEM(addr) || !IN_PMEM(addr + len)) {
        return EFAULT;
    }
    memcpy(GUEST_TO_HOST(addr), val, len);
    return 0;
}

static gdb_action_t emu_cont(void *args) {
    while (!emu_is_exited() && cpu.pc != semu_dbg_stat.bp_addr) {
        cpu_exec(1);
    }
    return ACT_RESUME;
}

static gdb_action_t emu_stepi(void *args) {
    if (!emu_is_exited()) {
        cpu_exec(1);
    }
    return ACT_RESUME;
}

static bool emu_set_bp(void *args, size_t addr, bp_type_t type) {
    dbg_stat_t *stat = (dbg_stat_t *)args;
    if (type != BP_SOFTWARE || stat->bp_is_set) {
        return false;
    }
    stat->bp_is_set = true;
    stat->bp_addr = addr;
    return true;
}

static bool emu_del_bp(void *args, size_t addr, bp_type_t type) {
    dbg_stat_t *stat = (dbg_stat_t *)args;
    if (type != BP_SOFTWARE || !stat->bp_is_set || stat->bp_addr != addr) {
        return true;
    }
    stat->bp_is_set = false;
    stat->bp_addr = (vaddr_t)0;
    return true;
}

static void emu_on_interrupt(void *args) {
  Info("Triggered interrupt");
  emu_stop();
}

static bool init_gdbstub() {
    emu_stop();
    if (!gdbstub_init(&semu_dbg_stat.gdbstub, &semu_ops, semu_arch_info, "127.0.0.1:1234")) {
        return false;
    }
    return true;
}

static void run_gdbstub() {
    Info("gdbstub is running...");
    gdbstub_run(&semu_dbg_stat.gdbstub, &semu_dbg_stat);
}

void start_gdbstub() {
    Log("Srarting gdbstub...");
    if (!init_gdbstub()) {
        Error("gdbstub init failed!");
        return;
    }
    run_gdbstub();
}

void stop_gdbstub() {
    Log("closing gdbstub...");
    gdbstub_close(&semu_dbg_stat.gdbstub);
    SET_STATE(QUIT);
}
