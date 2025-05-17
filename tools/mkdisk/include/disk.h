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

#include "disktype.h"

#include <stddef.h>
#include <unistd.h>

extern int disk_fd;
extern off_t logical_block_size;

extern enum partition_layout_type disk_type;

int open_disk(const char *path);
int seek_block(off_t lba);
int truncate_up(off_t total_sectors);
off_t get_total_sectors();
int identify_disk();

ssize_t read_disk(void *buf, size_t len);
ssize_t write_disk(const void *buf, size_t len);
