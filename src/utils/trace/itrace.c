#include <common.h>
#include <cpu/inst.h>

void itrace(exec_t *info) {
#ifdef CONFIG_ENABLE_ITRACE
    printf("0x%08" PRIaddr " : 0x%08" PRIx32 "\n", info->snpc, *(uint32_t *)&info->inst);
#endif
}
