#include <capstone/capstone.h>
#include <common.h>
#include <dlfcn.h>

csh (*cs_open_dl)(cs_arch, cs_mode, csh *);
cs_err (*cs_close_dl)(csh *);
size_t (*cs_disasm_dl)(csh, const uint8_t *, size_t, uint64_t, size_t, cs_insn **);
void (*cs_free_dl)(cs_insn *, size_t);

static bool init_success;
static csh handle_cs;

static void load_capstone() {
    void *handle = dlopen("tools/capstone/repo/libcapstone.so.6", RTLD_LAZY);

    if (!handle) {
        init_success = false;
        Error("dlopen failed: %s\n", dlerror());
        return;
    }

    cs_open_dl = dlsym(handle, "cs_open");
    cs_close_dl = dlsym(handle, "cs_close");
    cs_disasm_dl = dlsym(handle, "cs_disasm");
    cs_free_dl = dlsym(handle, "cs_free");
    assert(cs_open_dl && cs_close_dl && cs_disasm_dl && cs_free_dl);
}

static void configure_capstone() {
    if (!init_success) {
        return;
    }

    cs_err err = cs_open_dl(CS_ARCH_RISCV, CS_MODE_RISCV32, &handle_cs);
    assert(err == CS_ERR_OK);
}

void init_disasm() {
    init_success = true;
    load_capstone();
    configure_capstone();
    if (!init_success) {
        Error("disasm util initialization failed!");
    }
}

void close_disasm() {
    if (!init_success) {
        return;
    }
    cs_close_dl(&handle_cs);
}

void disasm(char *s, vaddr_t pc, const void *code, size_t code_size) {
    if (!init_success) {
        return;
    }
    
    cs_insn *insn = NULL;
    size_t count = cs_disasm_dl(handle_cs, code, code_size, pc, 0, &insn);
    assert(count == 1);
    sprintf(s, "0x%"PRIx64":\t%s\t\t%s\n", insn[0].address, insn[0].mnemonic, insn[0].op_str);
    cs_free_dl(insn, count);
}
