#include <memory.h>
#include <device/mmio.h>

#define NR_maps 16

static uint8_t iospace[CONFIG_MMIO_SPACE_SIZE] PG_ALIGN;
static uint8_t *p_space = NULL;

static IOMap maps[NR_maps];
static size_t nr_maps = 0;

void difftest_request_skip_step();

static inline void init_iospace() {
    p_space = (uint8_t *)ROUNDUP(iospace, PGSIZE);
}

static inline uint8_t *new_iospace(size_t size) {
    uint8_t *ret = p_space;
    p_space += size;
    p_space = (uint8_t *)ROUNDUP(p_space, PGSIZE);
    return ret;
}

static int mapcmp(const void *a, const void *b) {
    return ((IOMap *)a)->low > ((IOMap *)b)->low ? 1 : -1;
}

#define CHECK_OVERLAP(IOADDR) \
    if ((IOADDR) >= CONFIG_MBASE && (IOADDR) < CONFIG_MBASE + CONFIG_MSIZE) { Error("ioaddr 0x%" PRIaddr " overlapped!", (ioaddr_t)(IOADDR)); Assert(0); }

void *add_iomap(ioaddr_t start, size_t size, io_handler_t handler) {
    CHECK_OVERLAP(start);
    CHECK_OVERLAP(start + size);
    Assert(nr_maps < NR_maps);
    for (size_t i = 0; i < nr_maps; i++) {
        if ((start >= maps[i].low && start < maps[i].high) || (start + size >= maps[i].low && start + size <= maps[i].high)) {
            Error("overlapped");
            assert(0);
        }
    }
    void *ret = new_iospace(size);
    maps[nr_maps++] = (IOMap) { .low = start, .high = start + size - 1, .io_handler = handler, .space = ret };
    qsort(maps, nr_maps, sizeof(IOMap), mapcmp);
    Log("Added mmio map at 0x%" PRIaddr "", start);
    return ret;
}

__attribute__((always_inline))
static inline int findmap(ioaddr_t addr) {
    int left = 0;
    int right = nr_maps - 1;
    while (left <= right) {
        int mid = (left + right) >> 1;
        if (maps[mid].low <= addr) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    if (right >= 0 && right < nr_maps && maps[right].high >= addr && maps[right].low <= addr) {
        return right;
    }
    return -1;
}

word_t mmio_read(ioaddr_t addr, size_t len) {
    int mapidx = findmap(addr);
    Assert(mapidx != -1);
    size_t offset = addr - maps[mapidx].low;
    if (maps[mapidx].io_handler) {
        maps[mapidx].io_handler(MMIO_READ, offset, len);
    }
#ifdef CONFIG_ENABLE_DIFFTEST
    difftest_request_skip_step();
#endif
    return HOST_READ(maps[mapidx].space + offset, len);
}

void mmio_write(ioaddr_t addr, size_t len, word_t data) {
    int mapidx = findmap(addr);
    Assert(mapidx != -1);
    size_t offset = addr - maps[mapidx].low;
    HOST_WRITE(maps[mapidx].space + offset, len, data);
    if (maps[mapidx].io_handler) {
        maps[mapidx].io_handler(MMIO_WRITE, offset, len);
    }
#ifdef CONFIG_ENABLE_DIFFTEST
    difftest_request_skip_step();
#endif
}

void init_mmio() {
    init_iospace();
    Log("MMIO Initialized");
}

