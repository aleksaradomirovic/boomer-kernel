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

#include <stdio.h>

extern void *alloc_map;

int main() {
    unsigned char mmap[128] = { 0 };
    alloc_map = mmap;

    if(unclaim_memory(0) == 0) {
        fprintf(stderr, "unclaim(unclaimed) erroneously succeeded\n");
        return -1;
    }

    if(claim_memory(0) != 0) {
        fprintf(stderr, "claim(unclaimed) failed\n");
        return -1;
    }

    if(claim_memory(0) == 0) {
        fprintf(stderr, "claim(claimed) erroneously succeeded\n");
        return -1;
    }

    if(unclaim_memory(0) != 0) {
        fprintf(stderr, "unclaim(claimed) failed\n");
        return -1;
    }

    if(unclaim_memory(0) == 0) {
        fprintf(stderr, "unclaim(unclaimed) erroneously succeeded\n");
        return -1;
    }

    return 0;
}
