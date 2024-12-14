#include <memory.h>
#include <device/mmio.h>

#define NR_maps 16

static uint8_t iospace[CONFIG_MMIO_SPACE_SIZE] PG_ALIGN;
static uint8_t *p_space = NULL;

static IOMap maps[NR_maps];
static size_t nr_maps = 0;

static inline void init_iospace() {
    p_space = (uint8_t *)ROUNDUP(iospace, PGSIZE);
}

static inline uint8_t *new_iospace(size_t size) {
    uint8_t *ret = p_space;
    p_space += size;
    p_space = (uint8_t *)ROUNDUP(p_space, PGSIZE);
    return ret;
}

#define CHECK_OVERLAP(IOADDR) \
    if ((IOADDR) >= CONFIG_MBASE && (IOADDR) < CONFIG_MBASE + CONFIG_MSIZE) { Error("ioaddr 0x%" PRIaddr " overlapped!", (ioaddr_t)(IOADDR)); assert(0); }

void *add_iomap(ioaddr_t start, size_t size, io_handler_t handler) {
    CHECK_OVERLAP(start);
    CHECK_OVERLAP(start + size);
    assert(nr_maps < NR_maps);
    void *ret = new_iospace(size);
    maps[nr_maps++] = (IOMap) { .low = start, .high = start + size - 1, .io_handler = handler, .space = ret };
    Log("Added mmio map at 0x%" PRIaddr "", start);
    return ret;
}

static inline int findmap(ioaddr_t addr) {
    for (int i = 0; i < nr_maps; i++) {
        if (addr >= maps[i].low && addr < maps[i].high) {
            return i;
        }
    }
    return -1;
}

word_t mmio_read(ioaddr_t addr, size_t len) {
    int mapidx = findmap(addr);
    assert(mapidx != -1);
    size_t offset = addr - maps[mapidx].low;
    if (maps[mapidx].io_handler) {
        maps[mapidx].io_handler(MMIO_READ, offset, len);
    }
    return host_read(maps[mapidx].space + offset, len);
}

void mmio_write(ioaddr_t addr, size_t len, word_t data) {
    int mapidx = findmap(addr);
    assert(mapidx != -1);
    size_t offset = addr - maps[mapidx].low;
    host_write(maps[mapidx].space + offset, len, data);
    if (maps[mapidx].io_handler) {
        maps[mapidx].io_handler(MMIO_WRITE, offset, len);
    }
}

void init_mmio() {
    init_iospace();
    Log("MMIO Initialized");
}

