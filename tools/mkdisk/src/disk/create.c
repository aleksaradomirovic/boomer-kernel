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
#include "mkdisk.h"

#include <fcntl.h>
#include <unistd.h>

int vdisk_create(int argc, char **argv) {
    if(argc < 1) {
        throw_error(EINVAL, "create disk: too few arguments");
    }

    if((disk_fd = open(argv[0], O_WRONLY | O_CREAT | O_TRUNC, 00666)) == -1) {
        throw_error(errno, "create disk: %s", argv[0]);
    }

    return 0;
}
