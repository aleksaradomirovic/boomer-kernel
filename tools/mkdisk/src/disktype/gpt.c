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
#include "disk.h"
#include "disktype.h"

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define EFIMAGIC "EFI PART"

struct __attribute__((packed)) gpt_header {
    char efi_magic[8];

    uint16_t subversion;
    uint16_t version;

    uint32_t header_size;
    uint32_t header_crc;

    uint32_t reserved;

    uint64_t current_lba;
    uint64_t backup_lba;
    uint64_t first_usable_lba;
    uint64_t last_usable_lba;

    unsigned char guid[16];

    uint64_t partition_entries_lba;
    uint32_t partition_count;
    uint32_t partition_entry_size;
    uint32_t partition_crc;
};

_Static_assert(sizeof(struct gpt_header) == 92);

static int update_gpt_header(struct gpt_header *header) {
    size_t partition_area_size = header->partition_count * header->partition_entry_size;
    off_t partition_sector_count = partition_area_size / logical_block_size;
    if(partition_area_size % logical_block_size != 0) {
        partition_sector_count++;
    }
    if(partition_sector_count < 32) {
        partition_sector_count = 32;
    }

    header->first_usable_lba = header->partition_entries_lba + partition_sector_count;
    if(truncate_up(header->first_usable_lba + 1) != 0) {
        return -1;
    }

    off_t total_sectors = get_total_sectors();
    if(total_sectors == -1) {
        return -1;
    }

    header->last_usable_lba = total_sectors - 1;

    unsigned char partbuf[partition_area_size];
    if(seek_block(header->partition_entries_lba) != 0) {
        return -1;
    }

    if(read_disk(partbuf, partition_area_size) != partition_area_size) {
        return -1;
    }

    header->partition_crc = crc32(partbuf, partition_area_size);
    header->header_crc = 0;

    uint32_t header_crc = crc32(header, sizeof(struct gpt_header));
    header->header_crc = header_crc;

    return 0;
}

int __gpt_format() {
    struct gpt_header header = {
        .efi_magic = EFIMAGIC,

        .version = 1,
        .subversion = 0,

        .header_size = sizeof(struct gpt_header),
        // .header_crc = 0,

        .current_lba = 1,
        .backup_lba  = 0,
        
        .partition_entries_lba = 2,
        .partition_count       = 0,
        .partition_entry_size  = 128,
        // .partition_crc         = 0,
    };

    for(size_t i = 0; i < 16; i++) {
        header.guid[i] = rand();
    }

    if(update_gpt_header(&header) != 0) {
        return -1;
    }

    if(seek_block(1) != 0) {
        return -1;
    }

    if(write_disk(&header, sizeof(struct gpt_header)) != sizeof(struct gpt_header)) {
        return -1;
    }

    return 0;
}

int __gpt_identify() {
    off_t total_sectors = get_total_sectors();
    if(total_sectors == -1) {
        return -1;
    }

    if(total_sectors > 34) {
        if(seek_block(1) != 0) {
            return -1;
        }

        char magic[8];
        if(read_disk(magic, 8) != 8) {
            return -1;
        }

        if(memcmp(magic, EFIMAGIC, 8) == 0) {
            return 1;
        }
    }

    return 0;
}

int __gpt_verify() {
    errno = 0;
    if(__gpt_identify() != 1) {
        return -1;
    }

    if(seek_block(1) != 0) {
        return -1;
    }

    struct gpt_header header;
    if(read_disk(&header, sizeof(struct gpt_header)) != sizeof(struct gpt_header)) {
        return -1;
    }

    struct gpt_header test_header = header;
    if(update_gpt_header(&test_header) != 0) {
        return -1;
    }

    if(memcmp(&test_header, &header, sizeof(struct gpt_header)) != 0) {
        return -1;
    }

    return 0;
}
