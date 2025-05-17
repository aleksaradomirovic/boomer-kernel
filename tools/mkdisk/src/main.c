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

#include <argp.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

bool verbose = false;

static const struct argp_option opts[] = {
    { NULL, 0, NULL, 0, "Disk options:", -3 },
    { "blocksize", 'S', "SIZE", 0, "Declare logical block size", 0 },

    { NULL, 0, NULL, 0, "Common options:", -2 },
    { "verbose", 'v', NULL, 0, "Enable verbose output", 0 },
    { 0 }
};

static int args_parse(int key, char *arg, struct argp_state *state) {
    switch(key) {
        case 'S':
            {
                char *next;
                errno = 0;
                unsigned long size = strtoul(arg, &next, 10);
                if(errno == ERANGE || size > 0x2000) {
                    argp_failure(state, ERANGE, ERANGE, "invalid block size");
                } else if(*next > ' ') {
                    argp_failure(state, EINVAL, EINVAL, "invalid block size");
                }
                logical_block_size = size;
            }
            break;
        case 'v':
            verbose = true;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

static const struct argp args = {
    .options = opts,
    .parser = args_parse,
};

int main(int argc, char **argv) {
    srand(time(NULL));

    int idx, status;
    if((status = argp_parse(&args, argc, argv, 0, &idx, NULL)) != 0) {
        return status;
    }

    if(idx >= argc) {
        error(EINVAL, 0, "no command provided");
    }

    if(logical_block_size == 0) {
        logical_block_size = 512;
        print_info(0, "assuming logical block size of %jd", (intmax_t) logical_block_size);
    }

    char *cmd = argv[idx];
    if(strcmp(cmd, "create") == 0) {
        if(vdisk_create(argc - idx - 1, argv + idx + 1) != 0) {
            return errno;
        }
    } else if(strcmp(cmd, "format") == 0) {
        if(vdisk_format(argc - idx - 1, argv + idx + 1) != 0) {
            return errno;
        }
    } else if(strcmp(cmd, "verify") == 0) {
        if(vdisk_verify(argc - idx - 1, argv + idx + 1) != 0) {
            return (errno != 0) ? errno : -1;
        }
    } else {
        error(EINVAL, 0, "unknown command '%s'", cmd);
    }

    return 0;
}
