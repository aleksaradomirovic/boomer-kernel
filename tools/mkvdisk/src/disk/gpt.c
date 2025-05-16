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

#include "crc.h"
#include "disk/gpt.h"
#include "io.h"
#include "vdisk.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define GPT_VERSION 1
#define GPT_SUBVERSION 0

_Static_assert(CHAR_BIT == 8);

struct __attribute__((packed)) gpt_header_v1 {
    char magic[8];

    uint16_t subrevision;
    uint16_t revision;

    uint32_t header_size;
    uint32_t header_crc;

    char reserved[4];

    uint64_t my_lba;
    uint64_t alternate_lba;
    uint64_t first_usable_lba;
    uint64_t last_usable_lba;

    uint8_t guid[16];

    uint64_t partition_entry_lba;
    uint32_t partition_entry_count;
    uint32_t partition_entry_size;
    uint32_t partition_entry_crc;
};

_Static_assert(sizeof(struct gpt_header_v1) == 92);

static int calculate_gpt_crcs(struct gpt_header_v1 *header) {
    unsigned char buf[header->partition_entry_count * header->partition_entry_size];
    if(goto_lba(header->partition_entry_lba) != 0) {
        return -1;
    }

    if(readf(diskfd, buf, sizeof(buf)) != sizeof(buf)) {
        return -1;
    }

    header->partition_entry_crc = crc32(buf, sizeof(buf));

    header->header_crc = 0;
    uint32_t header_crc = crc32(header, sizeof(header));
    header->header_crc = header_crc;

    return 0;
}

int gpt_verify() {
    if(goto_lba(1) != 0) {
        return -1;
    }

    struct gpt_header_v1 header;
    if(readf(diskfd, &header, sizeof(header)) != sizeof(header)) {
        return -1;
    }

    struct gpt_header_v1 test_header = header;
    if(calculate_gpt_crcs(&test_header) != 0) {
        return -1;
    }

    if(memcmp(&header, &test_header, sizeof(header)) != 0) {
        return -1;
    }

    return 0;
}

int gpt_format() {
    struct gpt_header_v1 header = {
        .magic = "EFI PART",
        .revision = 1,
        .subrevision = 0,

        .header_size = sizeof(struct gpt_header_v1),
        // .header_crc = ?

        .my_lba = 1,
        .alternate_lba = 0,
        .first_usable_lba = 1 + 1 + 32,
        .last_usable_lba = get_disk_blocks() - 1,

        .partition_entry_lba = 2,
        .partition_entry_count = 0,
        .partition_entry_size = 128,
        // .partition_entry_crc = ?
    };

    srand(time(NULL));
    for(size_t i = 0; i < 16; i++) {
        header.guid[i] = rand();
    }

    if(calculate_gpt_crcs(&header) != 0) {
        return -1;
    }

    if(goto_lba(1) != 0) {
        return -1;
    }

    if(writef(diskfd, &header, sizeof(header)) != sizeof(header)) {
        return -1;
    }

    return 0;
}
