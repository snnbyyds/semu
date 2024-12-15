#include <device/mmio.h>

uint8_t *serial_base = NULL;

static void serial_io_handler(mmio_rw_t mmio_rw_op, size_t offset, size_t len) {
    Assert(len == 1 && offset == 0);
    Assert(serial_base);
    if (mmio_rw_op == MMIO_WRITE) {
        putchar(*serial_base);
    } else {
        Error("Read not implemented!");
    }
}

void init_serial() {
    serial_base = add_iomap(CONFIG_SERIAL_MMIO, 8, serial_io_handler);
}
