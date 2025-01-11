/*
 * Copyright 2025 Nuo Shen, Nanjing University
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cpu/cpu.h>
#include <memory.h>
#include <string.h>

/*
  Structure of a Semusnapshot File

  A Semusnapshot file is organized as follows:

  +-----------------------------------+ <- 0x0 (start)
  |            Header                 |
  |                                   |
  |  - magic/str (file identifier)    |
  |  - version (file version)         |
  |  - cpu_state_offset (offset to    |
  |    the CPU state structure)       |
  |  - mem_data_offset (offset to     |
  |    the memory data region)        |
  +-----------------------------------+ <- cpu_state_offset
  |                                   |
  |          CPU State                |
  |  (Registers, control flags, etc.) |
  |                                   |
  +-----------------------------------+ <- mem_data_offset
  |                                   |
  |          Memory State             |
  | (Memory data of size CONFIG_MSIZE)|
  |                                   |
  +-----------------------------------+ <- end

  Summary:
  A Semusnapshot encapsulates a snapshot of the emulated system, represented as:
  S = < R, M >
  Where:
    R: CPU state (registers and control information).
    M: Memory state (main memory content).

  Offsets in the header are specified in bytes relative to the start of the file.
*/

#define FILE_VERSION 2
#define MAGIC 0x50414e53554d4553ULL

/* Definition of the Semusnapshot header structure */
typedef struct __attribute__((packed)) {
  union {
    char str[8];      // SEMUSNAP
    uint64_t magic;   // 0x50414e53554d4553
  };
  uint8_t version;
  uint32_t cpu_state_offset;
  uint32_t mem_data_offset;
} snapshot_hdr_t;

static void *snapshot_buff = NULL;
static const size_t snapshot_buff_size = CONFIG_MSIZE + sizeof(CPU_State) + sizeof(snapshot_hdr_t);
static snapshot_hdr_t snapshot_file_hdr = { .magic = -1 };
static bool input_snapshot_valid = true;

#define CHECK_FILE(BUFFER) \
    do { \
        if (((snapshot_hdr_t *)BUFFER)->magic != MAGIC || \
            ((snapshot_hdr_t *)BUFFER)->version != FILE_VERSION) { \
            input_snapshot_valid = false; \
            Error("Invalid file"); \
        } \
    } while (0)

static inline void init_hdr(snapshot_hdr_t *hdr) {
    hdr->magic = MAGIC;
    hdr->version = FILE_VERSION;
    hdr->cpu_state_offset = sizeof(snapshot_hdr_t);
    hdr->mem_data_offset = sizeof(snapshot_hdr_t) + sizeof(CPU_State);
    snapshot_file_hdr = *hdr;
}

static void save_cpu_state() {
    *(CPU_State *)(snapshot_buff + snapshot_file_hdr.cpu_state_offset) = cpu;
    Info("Successfully saved CPU state");
}

static void save_mem_state() {
    memcpy(snapshot_buff + snapshot_file_hdr.mem_data_offset,
        GUEST_TO_HOST(CONFIG_MBASE),
        CONFIG_MSIZE);
    Info("Successfully saved Memory state");
}

static void write_snapshot(const char *path) {
    assert(snapshot_buff);
    FILE *file = fopen(path, "w+");
    if (file == NULL) {
        Error("snapshot creation failed!\n");
        return;
    }
    CHECK_FILE(snapshot_buff);
    fwrite(snapshot_buff, snapshot_buff_size, 1, file);
    fclose(file);
}

static void read_snapshot(const char *path) {
    assert(snapshot_buff != NULL);
    FILE *file = fopen(path, "r");
    if (file == NULL ||
        fread(snapshot_buff, snapshot_buff_size, 1, file) <= 0) {
        Warn("snapshot read failed!\n");
        input_snapshot_valid = false;
        return;
    }
    CHECK_FILE(snapshot_buff);
    fclose(file);
}

static void load_cpu_state() {
    if (!input_snapshot_valid) {
        return;
    }
    cpu = *(CPU_State *)(snapshot_buff + snapshot_file_hdr.cpu_state_offset);
    Info("CPU State loaded");
}

static void load_mem_state() {
    if (!input_snapshot_valid) {
        return;
    }
    memcpy(GUEST_TO_HOST(CONFIG_MBASE),
        snapshot_buff + snapshot_file_hdr.mem_data_offset,
        CONFIG_MSIZE);
    Info("Memory state loaded");
}

void save_snapshot(const char *path) {
    snapshot_buff = malloc(snapshot_buff_size);
    init_hdr(snapshot_buff);
    save_cpu_state();
    save_mem_state();
    write_snapshot(path);
    free(snapshot_buff);
    input_snapshot_valid = true; // reset
}

void load_snapshot(const char *path) {
    snapshot_buff = malloc(snapshot_buff_size);
    read_snapshot(path);
    snapshot_file_hdr = *(snapshot_hdr_t *)snapshot_buff;
    load_cpu_state();
    load_mem_state();
    free(snapshot_buff);
    input_snapshot_valid = true; // reset
}
