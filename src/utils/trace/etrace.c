#include <common.h>

void etrace(word_t no, vaddr_t epc) {
#ifdef CONFIG_ENABLE_ETRACE
    Warn("Triggered an interrupt/exception! no: %" PRIword " epc: 0x%08" PRIaddr "", no, epc);
#endif
}
