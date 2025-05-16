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

#include "vdisk.h"

#include <errno.h>
#include <limits.h>
#include <stdckdint.h>
#include <stddef.h>
#include <unistd.h>

[[maybe_unused]]
static off_t goto_lba(off_t lba) {
    off_t offset;
    if(ckd_mul(&offset, lba, logical_block_size)) {
        errno = ERANGE;
        return -1;
    }

    if(lseek(diskfd, offset, SEEK_SET) < 0) { // move to LBA 1
        return -1;
    }

    return 0;
}

[[maybe_unused]]
static off_t get_disk_size() {
    return lseek(diskfd, 0, SEEK_END);
}

[[maybe_unused]]
static off_t get_disk_blocks() {
    off_t size = get_disk_size();
    if(size < 0) {
        return -1;
    }
    
    return size / logical_block_size;
}

[[maybe_unused]]
static ssize_t readf(int fd, void *buf, size_t count) {
    if(count > SSIZE_MAX) {
        errno = EINVAL;
        return -1;
    }

    size_t total = 0;
    while(total < count) {
        ssize_t r = read(fd, buf + total, count - total);
        if(r < 0) {
            return -1;
        }
        if(r == 0) {
            break;
        }
        total += r;
    }

    return (ssize_t) total;
}

[[maybe_unused]]
static ssize_t writef(int fd, const void *buf, size_t count) {
    if(count > SSIZE_MAX) {
        errno = EINVAL;
        return -1;
    }

    size_t total = 0;
    while(total < count) {
        ssize_t r = write(fd, buf + total, count - total);
        if(r < 0) {
            return -1;
        }
        if(r == 0) {
            break;
        }
        total += r;
    }

    return (ssize_t) total;
}
