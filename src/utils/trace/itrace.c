#include <common.h>
#include <cpu/inst.h>

#ifdef CONFIG_ENABLE_ITRACE
static bool itrace_enable = true;
#endif

void itrace_switch(bool flag) {
#ifdef CONFIG_ENABLE_ITRACE
    itrace_enable = flag;
#endif
}

void itrace(exec_t *info, uint32_t inst) {
#ifdef CONFIG_ENABLE_ITRACE
    extern void disasm(char *s, vaddr_t pc, const void *code, size_t code_size);
    static char buffer[128];
    disasm(buffer, info->pc, &inst, sizeof(uint32_t));
    printf("%s", buffer);
#endif
}
