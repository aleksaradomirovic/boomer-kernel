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

#include "vdisk.h"

#include <errno.h>
#include <error.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

bool verbose = false;
char *diskname = NULL;
off_t logical_block_size = 0;

int diskfd;

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

const struct argp_child generic_args[] = {
    { &args, 0, NULL, 0 },
    { NULL }
};

int main(int argc, char **argv) {
    int idx, status;
    if((status = argp_parse(&args, argc, argv, ARGP_IN_ORDER, &idx, NULL)) != 0) {
        return status;
    }

    if(logical_block_size == 0) {
        logical_block_size = 512;
        if(verbose) {
            fprintf(stderr, "assuming logical block size of %jd\n", (intmax_t) logical_block_size);
        }
    }

    char *command;
    if(idx < argc) {
        command = argv[idx];
    } else {
        error(EINVAL, 0, "no command specified");
        exit(EINVAL);
    }

    char help_name[strlen(argv[0]) + strlen(command) + 2];
    strcpy(help_name, argv[0]);
    strcat(help_name, " ");
    strcat(help_name, command);
    argv[idx] = help_name;

    if(strcmp(command, "create") == 0) {
        return vdisk_create(argc - idx, argv + idx);
    } else if(strcmp(command, "verify") == 0) {
        return vdisk_verify(argc - idx, argv + idx);
    } else if(strcmp(command, "format") == 0) {
        return vdisk_format(argc - idx, argv + idx);
    } else {
        error(EINVAL, 0, "no such command '%s'", command);
    }

    return 0;
}
