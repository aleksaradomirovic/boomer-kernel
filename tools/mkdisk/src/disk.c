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

#include "disk.h"
#include "io.h"
#include "mkdisk.h"

#include <errno.h>
#include <error.h>
#include <fcntl.h>
#include <stdckdint.h>
#include <stdint.h>
#include <sys/stat.h>

int disk_fd = -1;
off_t logical_block_size = 0;

enum partition_layout_type disk_type;

int open_disk(const char *path) {
    if(disk_fd != -1) {
        errno = EBUSY;
        return -1;
    }

    disk_fd = open(path, O_RDWR);
    if(disk_fd == -1) {
        return -1;
    }

    return 0;
}

__attribute__((destructor))
static void close_disk() {
    if(disk_fd != -1) {
        if(close(disk_fd) != 0) {
            error(errno, errno, "failed to close disk file");
        }
        disk_fd = -1;
    }
}

int identify_disk() {
    int status = __gpt_identify();
    if(status == -1) {
        return -1;
    } else if(status == 1) {
        print_info(0, "deduced disk type as GPT");
        disk_type = LAYOUT_TYPE_GPT;
        return 0;
    }
    
    disk_type = LAYOUT_TYPE_UNKNOWN;
    errno = ENOTSUP;
    return -1;
}

int seek_block(off_t lba) {
    off_t offset;
    if(ckd_mul(&offset, lba, logical_block_size)) {
        errno = ERANGE;
        return -1;
    }

    if(truncate_up(lba + 1) != 0) {
        return -1;
    }

    if(lseek(disk_fd, offset, SEEK_SET) == -1) {
        return -1;
    }

    return 0;
}

int truncate_up(off_t total_sectors) {
    off_t offset;
    if(ckd_mul(&offset, total_sectors, logical_block_size)) {
        errno = ERANGE;
        return -1;
    }
    
    off_t end = lseek(disk_fd, 0, SEEK_END);
    if(end == -1) {
        return -1;
    }

    if(offset <= end) {
        return 0;
    }

    print_info(0, "truncating disk image to %jd bytes (%jd sectors)", (intmax_t) offset, (intmax_t) total_sectors);
    if(ftruncate(disk_fd, offset) != 0) {
        return -1;
    }

    return 0;
}

static off_t get_disk_size() {
    return lseek(disk_fd, 0, SEEK_END);
}

off_t get_total_sectors() {
    off_t size = get_disk_size();
    if(size == -1) {
        return -1;
    }

    return size / logical_block_size;
}

ssize_t read_disk(void *buf, size_t len) {
    return readf(disk_fd, buf, len);
}

ssize_t write_disk(const void *buf, size_t len) {
    return writef(disk_fd, buf, len);
}
