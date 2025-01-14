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

/* REF: https://github.com/ksco/rvemu/blob/main/src/compile.c */

#include <jit.h>
#include <elf.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>

#define __USE_GNU
#include <pthread.h>

#define MAX_ELFSZ 0x10000

static uint8_t elfbuf[MAX_ELFSZ];

static uint8_t *postprocess_elf(cache_t *cache, vaddr_t pc) {
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)elfbuf;

    Elf64_Off text_idx = 0;
    Elf64_Off symtab_idx = 0;
    Elf64_Off rela_idx = 0;
    Elf64_Off rodata_idx = 0;
    Elf64_Off shstr_shoff = ehdr->e_shoff + ehdr->e_shstrndx * sizeof(Elf64_Shdr);
    Elf64_Shdr *shstr_shdr = (Elf64_Shdr *)(elfbuf + shstr_shoff);
    assert(ehdr->e_shnum);

    for (Elf64_Off i = 0; i < ehdr->e_shnum; i++) {
        Elf64_Off shoff = ehdr->e_shoff + i * sizeof(Elf64_Shdr);
        Elf64_Shdr *shdr = (Elf64_Shdr *)(elfbuf + shoff);
        const char *str = (const char *)(elfbuf + shstr_shdr->sh_offset + shdr->sh_name);
        if (!strcmp(str, ".text")) {
            text_idx = i;
        } else if (!strcmp(str, ".rela.text")) {
            rela_idx = i;
        } else if (!strncmp(str, ".rodata.", 8)) {
            rodata_idx = i;
        } else if (!strcmp(str, ".symtab")) {
            symtab_idx = i;
        }
    }
    assert(text_idx && symtab_idx);

    Elf64_Off text_shoff = ehdr->e_shoff + text_idx * sizeof(Elf64_Shdr);
    Elf64_Shdr *text_shdr = (Elf64_Shdr *)(elfbuf + text_shoff);
    if (!rela_idx || !rodata_idx) {
        return jit_cache_add(cache, pc, elfbuf + text_shdr->sh_offset,
            text_shdr->sh_size, text_shdr->sh_addralign);
    }

    uintptr_t text_addr = 0;
    uintptr_t rodata_addr = 0;
    Elf64_Off shoff = ehdr->e_shoff + rodata_idx * sizeof(Elf64_Shdr);
    Elf64_Shdr *shdr = (Elf64_Shdr *)(elfbuf + shoff);
    rodata_addr = (uintptr_t)jit_cache_add(cache, pc, elfbuf + shdr->sh_offset,
        shdr->sh_size, shdr->sh_addralign);
    text_addr = (uintptr_t)jit_cache_add(cache, pc, elfbuf + text_shdr->sh_offset,
        text_shdr->sh_size, text_shdr->sh_addralign);

    shoff = ehdr->e_shoff + rela_idx * sizeof(Elf64_Shdr);
    shdr = (Elf64_Shdr *)(elfbuf + shoff);
    uint64_t rels = shdr->sh_size / sizeof(Elf64_Rela);
    Elf64_Off symtab_shoff = ehdr->e_shoff + symtab_idx * sizeof(Elf64_Shdr);
    Elf64_Shdr *symtab_shdr = (Elf64_Shdr *)(elfbuf + symtab_shoff);

    for (uint64_t i = 0; i < rels; i++) {
        Elf64_Rela *rel = (Elf64_Rela *)(elfbuf + shdr->sh_offset + i * sizeof(Elf64_Rela));
        Elf64_Sym *sym = (Elf64_Sym *)(elfbuf + symtab_shdr->sh_offset + (rel->r_info >> 32) * sizeof(Elf64_Sym));

        uint32_t *loc = (uint32_t *)(text_addr + rel->r_offset);
        uint64_t S = rodata_addr + sym->st_value;
        uint64_t P = text_addr + rel->r_offset;
        int64_t A = rel->r_addend;
        *loc = (uint32_t)(S + A - P);
        uint64_t rip_addr = P - A;
        assert(rip_addr + *(int32_t *)loc == S);
    }

    return (uint8_t *)text_addr;
}

uint8_t *jit_compiler(code_block_t *code, cache_t *cache, vaddr_t pc) {
    puts(code->c_code);
    int saved_stdout = dup(STDOUT_FILENO);
    int outp[2];

    int res = pipe(outp);
    if (unlikely(res)) {
        Panic("Failed to make a pipe!");
    }
    dup2(outp[1], STDOUT_FILENO);
    close(outp[1]);
 
    FILE *f = popen("clang -O3 -c -xc -o /dev/stdout -", "w");
    if (!f) {
        Panic("Failed to compile code");
    }
    fwrite(code->c_code, 1, code->len, f);
    assert(0);
    pclose(f);
    fflush(stdout);

    (void)read(outp[0], elfbuf, MAX_ELFSZ);
    close(outp[0]);
    dup2(saved_stdout, STDOUT_FILENO);
    close(saved_stdout);
    return postprocess_elf(cache, pc);
}
