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

#pragma once

#include <limits.h>
#include <stddef.h>
#include <stdint.h>

#define CRC32POLY 0xEDB88320
#define CRC32SIZE (1 << CHAR_BIT)

_Static_assert(UINT32_MAX <= SIZE_MAX);

static void crc32_lookup_init(uint32_t *lookup) {
    for(size_t i = 0; i < CRC32SIZE; i++) {
        uint32_t crc = i;
        for(unsigned char j = 0; j < 8; j++) {
            if(crc & 0b1) {
                crc = (crc >> 1) ^ ~CRC32POLY;
            } else {
                crc >>= 1;
            }
        }
        lookup[i] = crc;
    }
}

[[maybe_unused]]
static uint32_t crc32(void *crcbuf, size_t len) {
    unsigned char *buf = crcbuf;

    const uint32_t lookup[CRC32SIZE];
    crc32_lookup_init((uint32_t *) lookup);

    uint32_t crc = ~0;
    for(size_t i = 0; i < len; i++) {
        size_t idx = (crc ^ buf[i]) & ((1 << CHAR_BIT) - 1);
        crc = (crc >> 8) ^ lookup[idx];
    }

    return crc;
}
