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

#include <cpu/jit.h>
#include <utils/hash.h>
#include <string.h>
#include <sys/mman.h>

/*
* JIT cache util
* Inspired by https://github.com/ksco/rvemu/blob/main/src/cache.c
*/

#define sys_icache_invalidate(addr, size) \
    __builtin___clear_cache((char *)(addr), (char *)(addr) + (size));

cache_t *jitcache_new() {
    cache_t *ret = (cache_t *)calloc(1, sizeof(cache_t));
    ret->jitcode = mmap(NULL, CACHE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC,
        MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    return ret;
}

uint8_t *jitcache_find(cache_t *cache, vaddr_t pc) {
    uint64_t idx = hash(pc, CACHE_ENTRY_SIZE);
    while (cache->table[idx].pc) {
        if (cache->table[idx].pc == pc) {
            if (cache->table[idx].hot >= CACHE_HOT) {
                return cache->jitcode + cache->table[idx].offset;
            }
            break;
        }
        idx = (idx + 1) % CACHE_ENTRY_SIZE;
    }
    return NULL;
}

uint8_t *jitcache_add(cache_t *cache, vaddr_t pc, const void *code, size_t size, uint64_t align) {
    cache->offset = cache->offset ? ROUNDUP(cache->offset, align) : 0;
    assert(cache->offset + size <= CACHE_SIZE);
    uint64_t idx = hash(pc, CACHE_ENTRY_SIZE);
    while (cache->table[idx].pc) {
        if (cache->table[idx].pc == pc) {
            break;
        }
        idx = (idx + 1) % CACHE_ENTRY_SIZE;
    }
    memcpy(cache->jitcode + cache->offset, code, size);
    cache->table[idx].pc = pc;
    cache->table[idx].offset = cache->offset;
    cache->offset += size;
    sys_icache_invalidate(cache->jitcode + cache->table[idx].offset, size);
    return cache->jitcode + cache->table[idx].offset;
}

bool jitcache_hot(cache_t *cache, vaddr_t pc) {
    uint64_t idx = hash(pc, CACHE_ENTRY_SIZE);
    while (cache->table[idx].pc) {
        if (cache->table[idx].pc == pc) {
            cache->table[idx].hot++;
            return cache->table[idx].hot >= CACHE_HOT;
        }
        idx = (idx + 1) % CACHE_ENTRY_SIZE;
    }
    cache->table[idx].pc = pc;
    cache->table[idx].hot = 1;
    return false;
}
