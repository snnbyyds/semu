#include <memory.h>
#include <cpu/cpu.h>
#include <cpu/inst.h>
#include <device/device.h>
#include <device/timer.h>
#include <pthread.h>
#include <string.h>
#include <utils/state.h>

CPU_State cpu = {};
uint64_t running_seconds = 0;

static pthread_t thread_cpu_exec;
static pthread_attr_t attr;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static struct sched_param param;
static volatile bool cpu_thread_finished = true;

static uint64_t step_count = 0;

static const uint32_t builtin_img[] = {
    0x00000297, // auipc t0,0
    0x00028823, // sb  zero,16(t0)
    0x0102c503, // lbu a0,16(t0)
    0x00100073, // ebreak
    0x00000000
};

__attribute__((always_inline))
static inline void exec_once() {
    // exec the inst
    extern void inst_exec_once(exec_t *);
    exec_t exec_info = { .snpc = cpu.pc, .pc = cpu.pc };
    inst_exec_once(&exec_info);
    cpu.pc = exec_info.dnpc;
}

// cpu_exec in child thread, arg as the step
static void *cpu_exec_thread(void *arg) {
    uint64_t step = (uint64_t)arg;
    uint64_t i = 0;
    for (; i < step; i++) {
        exec_once();
        if (semu_state.state != RUNNING) {
            break;
        }
    }
    step_count += i;

    pthread_mutex_lock(&mutex);
    cpu_thread_finished = true;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

static void start_cpu_exec_thread(uint64_t nr_exec) {
    cpu_thread_finished = false;
    if (pthread_create(&thread_cpu_exec, &attr, cpu_exec_thread, (void *)nr_exec)) {
        cpu_thread_finished = true;
        Error("Failed to create cpu_exec thread!");
        assert(0);
    }
}

static void wait_cpu_exec_thread() {
    pthread_mutex_lock(&mutex);
    while (!cpu_thread_finished) {
        update_device();
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);
    pthread_join(thread_cpu_exec, NULL);
}

static void halt_cpu() {
    stop_timers();
    pthread_attr_destroy(&attr);
    Info("%" PRIu64 " inst simulated", step_count);
    if (running_seconds > 4) {
        Info("Simulation Speed: %" PRIu64 " inst/s", step_count / running_seconds);
    }
}

void init_cpu(bool img_builtin) {
    memset(&cpu, 0, sizeof(cpu));
    cpu.pc = CONFIG_RESET_VECTOR;

    // Set up thread
    pthread_attr_init(&attr);
    pthread_attr_setschedpolicy(&attr, SCHED_RR);
    param.sched_priority = sched_get_priority_max(SCHED_RR);
    pthread_attr_setschedparam(&attr, &param);
    Log("Use SCHED_RR for cpu_exec_thread %p", cpu_exec_thread);
    
    init_inst_pool();
    Log("CPU Initialized!");

    // Load builtin image
    if (img_builtin) {
        Log("Loading builtin image to 0x%" PRIx64 "...", (uint64_t)CONFIG_RESET_VECTOR);
        memcpy(guest_to_host(CONFIG_RESET_VECTOR), builtin_img, sizeof(builtin_img));
    }
}

void cpu_exec(uint64_t step) {
    switch (semu_state.state) {
        case END: case ABORT: case QUIT: stop_timers(); printf("Program has ended. Please restart semu.\n"); return;
        default: SET_STATE(RUNNING); resume_timers(); break;
    }

    start_cpu_exec_thread(step);
    wait_cpu_exec_thread();

    switch (semu_state.state) {
        case RUNNING: SET_STATE(STOP); stop_timers(); break;
        case END: Info("Hit Good Trap at PC 0x%08" PRIaddr "", semu_state.halt_pc); halt_cpu(); break;
        case ABORT: Error("Hit Bad Trap at PC 0x%08" PRIaddr "", cpu.pc); halt_cpu(); break;
        case QUIT: halt_cpu(); break;
        default: break;
    }
}
