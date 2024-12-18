#include <device/device.h>
#include <device/mmio.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

#define CHECK_BLKNO(NO) ({ assert((NO) < disk_base[reg_blkcnt]); })

enum {
  reg_present,
  reg_blksz,
  reg_blkcnt,
  reg_ready,
  reg_cmd,
  reg_blkno,
  nr_reg
};

static char diskpath[64] = "";
static long disksize = -1;
static FILE *disk = NULL;
static uint32_t *disk_base = NULL;
static bool disk_present = false;
static uint8_t *blkbuf = NULL;

// This indicates whether needed param has been configured in this IO circle
static bool received_blkno = false;

static inline void get_disk_path() {
    sprintf(diskpath, "%s/resource/disk.img", getenv("SEMU_HOME"));
    if (access(diskpath, F_OK) != 0) {
        char cmd[128] = "";
        sprintf(cmd, "dd if=/dev/random of=%s bs=1M count=16", diskpath);
        int res = system(cmd);
        assert(res == 0);
    }
}

static inline void get_disksize() {
    struct stat st;
    if (!stat(diskpath, &st)) {
        disk_present = true;
        disksize = st.st_size;
    } else {
        Error("Failed to stat %s", diskpath);
        disk_present = false;
    }
}

__attribute__((always_inline))
static inline void disk_read() {
    if (!disk_present) {
        return;
    }
    assert(received_blkno);
    disk_base[reg_ready] = false;
    fseek(disk, CONFIG_DISK_BLK_SZ * disk_base[reg_blkno], SEEK_SET);
    if (fread(blkbuf, 1, CONFIG_DISK_BLK_SZ, disk) <= 0) {
        Error("Read Failed");
    }
    disk_base[reg_ready] = true;
}

__attribute__((always_inline))
static inline void disk_write() {
    if (!disk_present) {
        return;
    }
    assert(received_blkno);
    disk_base[reg_ready] = false;
    fseek(disk, CONFIG_DISK_BLK_SZ * disk_base[reg_blkno], SEEK_SET);
    if (fwrite(blkbuf, 1, CONFIG_DISK_BLK_SZ, disk) <= 0) {
        Error("Write failed");
    }
    disk_base[reg_ready] = true;
}

static void disk_io_handler(mmio_rw_t mmio_rw_op, size_t offset, size_t len) {
    assert((offset & 0x3) == 0 && offset < sizeof(uint32_t) * nr_reg);
    if (mmio_rw_op == MMIO_WRITE) {
        uint32_t reg_no = offset >> 2;
        switch (reg_no) {
            case reg_cmd: disk_base[reg_cmd] ? disk_write() : disk_read(); break;
            case reg_blkno: received_blkno = true; CHECK_BLKNO(disk_base[reg_no]); break;
            default: Warn("Unexpeted write!"); break;
        }
    }
}

void open_disk() {
    get_disk_path();
    disk = fopen(diskpath, "r+");
    get_disksize();
    Log("Successfully opened a 0x%lx disk at %s!", disksize, diskpath);
}

void close_disk() {
    if (disk) {
        fclose(disk);
    }
}

void init_disk() {
    open_disk();
    disk_base = add_iomap(CONFIG_DISK_CTL_MMIO, nr_reg * sizeof(uint32_t), disk_io_handler);
    blkbuf = add_iomap(CONFIG_DB_ADDR, CONFIG_DISK_BLK_SZ, NULL);
    disk_base[reg_present] = disk_present;
    disk_base[reg_blksz] = CONFIG_DISK_BLK_SZ;
    disk_base[reg_blkcnt] = disksize / CONFIG_DISK_BLK_SZ;
    disk_base[reg_ready] = disk_present;
    disk_base[reg_cmd] = 0;
    disk_base[reg_blkno] = 0;
}
