#include <cpu/cpu.h>
#include <memory.h>
#include <device/device.h>
#include <device/mmio.h>
#include <utils/difftest.h>
#include <utils/state.h>
#include <utils/timer.h>
#include <getopt.h>
#include <string.h>

void init_cpu(bool img_builtin);
void init_disasm();
void init_memory();
void sdb_batchmode_on();

size_t image_size = -1;
extern size_t builtin_img_size;

static char *image = NULL;

static void parse_args(int argc, char *argv[]) {
    const struct option options[] = {
        {"help", no_argument, NULL, 'h'},
        {"version", no_argument, NULL, 'v'},
        {"batch", no_argument, NULL, 'b'},
        {0, 0, 0, 0}
    };
    int opt;
    while (1) {
        opt = getopt_long(argc, argv, "-hvb", options, NULL);
        if (opt == -1) {
            break;
        }
        switch (opt) {
            case 'v': printf("Version: %s\n", tostr(VERSION)); exit(EXIT_SUCCESS);
            case 'b': sdb_batchmode_on(); break;
            case 1: image = optarg; break;
            default:
                // TODO: Better prompt
                puts("Valid Options:");
                for (size_t i = 0; options[i].name; i++) {
                    printf("--%-6s\n", options[i].name);
                }
                exit(EXIT_SUCCESS);
        }
    }
}

static void welcome() {
    puts("\033[1;34;47m   _____ ______ __  __ _    _    \033[0m");
    puts("\033[1;34;47m   / ____|  ____|  \\/  | |  | |  \033[0m");
    puts("\033[1;34;47m  | (___ | |__  | \\  / | |  | |  \033[0m");
    puts("\033[1;34;47m   \\___ \\|  __| | |\\/| | |  | |  \033[0m");
    puts("\033[1;34;47m   ____) | |____| |  | | |__| |  \033[0m");
    puts("\033[1;34;47m  |_____/|______|_|  |_|\\____/   \033[0m");
    puts("\033[1;34;47m                                 \033[0m");
    putchar('\n');
    printf("\033[35;47mWelcome to SEMU v%s!\033[0m\n", tostr(VERSION));
    printf("Build time: %s, %s\n", __TIME__, __DATE__);
#ifdef CONFIG_USE_GDBSTUB
    puts("Please connect GDB.");
#else
    printf("Type '\033[32mhelp\033[0m' for help.\n");
#endif
}

static void load_image() {
    if (!image) {
        return;
    }
    FILE *fp = fopen(image, "rb");
    assert(fp);
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    Log("Loading image %s of size %ld...", image, size);
    fseek(fp, 0, SEEK_SET);
    int res = fread(GUEST_TO_HOST(CONFIG_RESET_VECTOR), size, 1, fp);
    assert(res == 1);
    fclose(fp);
    image_size = size;
}

void init_monitor(int argc, char *argv[]) {
    get_boot_time();
    parse_args(argc, argv);
    SET_STATE(STOP);
    init_memory();
    init_cpu(image == NULL);
    init_mmio();
    init_device();
    load_image();
#ifdef CONFIG_ENABLE_ITRACE
    init_disasm();
#endif /* CONFIG_ENABLE_ITRACE */
#ifdef CONFIG_DIFFTEST
#ifdef CONFIG_DIFFTEST_REF_SPIKE
    init_difftest(
        "tools/spike-diff/build/riscv32-spike-so",
        0,
        image_size == -1 ? builtin_img_size : image_size
    );
#else
    init_difftest(
        "tools/nemu-diff/repo/build/riscv32-nemu-interpreter-so",
        0,
        image_size == -1 ? builtin_img_size : image_size
    );
#endif
#endif /* CONFIG_ENABLE_DIFFTEST */
    welcome();
}
