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

#include "mkdisk.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

void print_info(unsigned int flags, const char *argfmt, ...) {
    char fmt[strlen(argfmt) + 2];
    strcpy(fmt, argfmt);
    strcat(fmt, "\n");

    va_list args;
    va_start(args, argfmt);
    if(verbose || (flags & PRINT_INFO_ALWAYS)) {
        vfprintf(stderr, fmt, args);
    }
    va_end(args);
}
