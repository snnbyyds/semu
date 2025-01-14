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

#include <jit.h>
#include <utils/queue.h>

#define __USE_GNU
#include <pthread.h>

static pthread_t translater_thread;
static pthread_attr_t attr;
static struct sched_param param;
static volatile queue32_t pc_queue;
static volatile bool stop_translater_thread = false;

static void *translater_child_thread(void *arg) {
    while (!stop_translater_thread) {
        if (!queue32_is_empty(&pc_queue)) {
            cache_t *jit_cache = get_jit_cache();
            if (jit_cache->status == cache_busy) {
                continue;
            }
            jit_cache->status = cache_busy;
            vaddr_t pc = queue32_dequeue(&pc_queue);
            code_block_t *src = jit_gen_c_code(pc);
            uint8_t *code = jit_compiler(src, jit_cache, pc);
            jit_cache->status = cache_free;
            assert(code);
        }
    }
    return NULL;
}

void jit_translater_stop() {
    Log("Translater thread was stopped.");
    stop_translater_thread = true;
}

void jit_translate(vaddr_t pc) {
    assert(!queue32_is_full(&pc_queue));
    queue32_enqueue(&pc_queue, pc);
}

void jit_init_translater() {
    queue32_init(&pc_queue);

    pthread_attr_init(&attr);
    pthread_attr_setschedpolicy(&attr, SCHED_RR);
    param.sched_priority = sched_get_priority_max(SCHED_RR) - 2; // FIXME
    pthread_attr_setschedparam(&attr, &param);

    int res = pthread_create(&translater_thread, &attr,
        translater_child_thread, NULL);
    if (unlikely(res)) {
        Panic("Failed to create translater thread!");
    }
}
