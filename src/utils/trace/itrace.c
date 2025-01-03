#include <common.h>
#include <cpu/inst.h>

void itrace(exec_t *info, uint32_t inst) {
#ifdef CONFIG_ENABLE_ITRACE
    extern void disasm(char *s, vaddr_t pc, const void *code, size_t code_size);
    static char buffer[128];
    disasm(buffer, info->pc, &inst, sizeof(uint32_t));
    printf("%s", buffer);
#endif
}
