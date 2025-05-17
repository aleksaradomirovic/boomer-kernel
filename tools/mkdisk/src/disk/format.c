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
#include "disktype.h"
#include "mkdisk.h"

#include <string.h>

int vdisk_format(int argc, char **argv) {
    if(argc < 2) {
        throw_error(EINVAL, "format: too few arguments");
    }

    if(open_disk(argv[0]) != 0) {
        if(errno == ENOENT) {
            char *subargv[] = { argv[0], NULL };
            if(vdisk_create(1, subargv) != 0) {
                return -1;
            }
            if(open_disk(argv[0]) != 0) {
                throw_error(errno, "format: %s", argv[0]);
            }
        } else {
            throw_error(errno, "format: %s", argv[0]);
        }
    }

    if(strcmp(argv[1], "gpt") == 0) {
        return __gpt_format();
    } else {
        throw_error(ENOTSUP, "format: %s", argv[1]);
    }
}
