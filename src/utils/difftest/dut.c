#include <dlfcn.h>
#include <cpu/cpu.h>
#include <cpu/reg.h>
#include <memory.h>
#include <utils/state.h>

// Note that gpr and pc must be in consistent with our CPU_State
typedef struct {
    word_t gpr[32];
    word_t pc;
} NEMU_CPU_Status;

// May adjust according to different refs in the future.
typedef NEMU_CPU_Status ref_context;

extern size_t image_size;
extern size_t builtin_img_size;

static void (*ref_difftest_memcpy)(paddr_t addr, void *buf, size_t n, bool direction) = NULL;
static void (*ref_difftest_regcpy)(void *dut, bool direction) = NULL;
static void (*ref_difftest_exec)(uint64_t n) = NULL;
static void (*ref_difftest_raise_intr)(uint64_t NO) = NULL;
static void (*ref_difftest_init)(int port) = NULL;

static bool to_dut = -1;
static bool to_ref = -1;

static bool ref_skip = false;

static const char *ref_so = "tools/nemu-diff/repo/build/riscv32-nemu-interpreter-so";

static void difftest_load_ref_so() {
    void *handle_ref = dlopen(ref_so, RTLD_LAZY);
    assert(handle_ref);

    ref_difftest_memcpy = dlsym(handle_ref, "difftest_memcpy");
    ref_difftest_regcpy = dlsym(handle_ref, "difftest_regcpy");
    ref_difftest_exec = dlsym(handle_ref, "difftest_exec");
    ref_difftest_raise_intr = dlsym(handle_ref, "difftest_raise_intr");
    ref_difftest_init = dlsym(handle_ref, "difftest_init");
    assert(ref_difftest_memcpy && ref_difftest_memcpy && ref_difftest_exec && ref_difftest_raise_intr && ref_difftest_init);

    const void *res = NULL;

    res = dlsym(handle_ref, "to_dut");
    assert(res);
    to_dut = (bool)*(uint32_t *)res;

    res = dlsym(handle_ref, "to_ref");
    assert(res);
    to_ref = (bool)*(uint32_t *)res;

    assert(to_dut != to_ref);

    Log("Loaded ref_so at %s", ref_so);
}

static void difftest_sync_image() {
    ref_difftest_memcpy(CONFIG_RESET_VECTOR, guest_to_host(CONFIG_RESET_VECTOR), image_size == -1 ? builtin_img_size : image_size, to_ref);
}

static void difftest_sync_context() {
    ref_difftest_regcpy(&cpu, to_ref);
    ref_difftest_memcpy(CONFIG_MBASE, guest_to_host(CONFIG_MBASE), CONFIG_MSIZE, to_ref);
}

static bool difftest_check_reg() {
    static ref_context ref_cpu = {};

    bool ret = true;
    ref_difftest_regcpy(&ref_cpu, to_dut);
    if (cpu.pc != ref_cpu.pc) {
        Error("Wrong PC!");
        printf("ref: 0x%08x | dut: 0x%08x\n", ref_cpu.pc, cpu.pc);
        ret = false;
    }
    for (size_t i = 0; i < NR_GPR; i++) {
        if (ref_cpu.gpr[i] != gpr(i)) {
            Error("register value not match for %s at pc 0x%08x", reg_val_to_name(i), cpu.pc);
            printf("ref: 0x%08x | dut: 0x%08x\n", ref_cpu.gpr[i], gpr(i));
            ret = false;
        }
    }
    return ret;
}

static void difftest_sync_intr() {
    /* Sync intr status */
    // TODO
}

void difftest_request_skip_step() {
    ref_skip = true;
}

void difftest_exec_once() {
    if (ref_skip) {
        ref_skip = false;
        difftest_sync_context();
        return;
    }
    ref_difftest_exec(1);
    if (!difftest_check_reg()) {
        SET_STATE(ABORT);
        return;
    }
    difftest_sync_intr();
}

void init_difftest() {
    Info("Initializing difftest...");
    Warn("This will significantly reduce the performance.");

    difftest_load_ref_so();
    ref_difftest_init(-1);
    difftest_sync_image();
    difftest_sync_context();
}
