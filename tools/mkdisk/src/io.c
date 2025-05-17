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

#include "io.h"

#include <errno.h>
#include <limits.h>

ssize_t readf(int fd, void *buf, size_t count) {
    if(count > SSIZE_MAX) {
        errno = ERANGE;
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

ssize_t writef(int fd, const void *buf, size_t count) {
    if(count > SSIZE_MAX) {
        errno = ERANGE;
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
