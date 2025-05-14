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
