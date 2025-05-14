/*
 * Copyright (C) 2025  Aleksa Radomirovic
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "memory/alloc.h"

#include <limits.h>
#include <stdatomic.h>

typedef _Atomic uint_fast8_t alloc_grain_t;
#define ALLOC_GRAIN_BITS (sizeof(alloc_grain_t) * CHAR_BIT)

alloc_grain_t *alloc_map;
paddr_t alloc_max_addr;

int claim_memory(paddr_t addr) {
    paddr_t grain_index = addr / ALLOC_GRANULARITY;
    paddr_t idx = grain_index / ALLOC_GRAIN_BITS;
    alloc_grain_t bit = 1 << (grain_index % ALLOC_GRAIN_BITS);

    if(atomic_fetch_or_explicit(&alloc_map[idx], bit, memory_order_acquire) & bit) {
        return -1;
    }

    return 0;
}

int unclaim_memory(paddr_t addr) {
    paddr_t grain_index = addr / ALLOC_GRANULARITY;
    paddr_t idx = grain_index / ALLOC_GRAIN_BITS;
    alloc_grain_t bit = 1 << (grain_index % ALLOC_GRAIN_BITS);

    if(!(atomic_fetch_and_explicit(&alloc_map[idx], ~bit, memory_order_acquire) & bit)) {
        return -1;
    }

    return 0;
}

int acquire_memory(paddr_t *memory, size_t count) {
    size_t acquired_count = 0;
    paddr_t acquired[count];

    for(paddr_t addr = 0; acquired_count < count && addr < alloc_max_addr; addr++) {
        if(claim_memory(addr) == 0) {
            acquired[acquired_count++] = addr;
        }
    }

    if(acquired_count == count) {
        for(size_t i = 0; i < count; i++) {
            memory[i] = acquired[i];
        }

        return 0;
    } else {
        release_memory(acquired, acquired_count);
    }

    return -1;
}

int release_memory(const paddr_t *memory, size_t count) {
    int status = 0;

    for(size_t i = 0; i < count; i++) {
        if(memory[i] >= alloc_max_addr || unclaim_memory(memory[i]) != 0) {
            status = -1;
        }
    }

    return status;
}
