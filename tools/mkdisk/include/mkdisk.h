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

#include <errno.h>
#include <error.h>

#define throw_error(num, fmt, ...) { \
    errno = num; \
    error(0, errno, fmt __VA_OPT__(,) __VA_ARGS__); \
    return -1; \
}

extern bool verbose;

int vdisk_create(int argc, char **argv);
int vdisk_format(int argc, char **argv);
int vdisk_verify(int argc, char **argv);

#define PRINT_INFO_ALWAYS (1 << 0)
void print_info(unsigned int flags, const char *fmt, ...);
