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

#include "disk/gpt.h"
#include "disk/mbr.h"
#include "io.h"
#include "vdisk.h"

#include <errno.h>
#include <error.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

static const struct argp_option opts[] = {
    { 0 }
};

static int args_parse(int key, char *args, struct argp_state *state) {
    switch(key) {
        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

static const struct argp args = {
    .options = opts,
    .parser = args_parse,
    .children = generic_args,
    .args_doc = "DISK_NAME FORMAT_TYPE",
    .doc = "Format the disk to use the specified partitioning layout.",
};

static char *format;

int vdisk_format(int argc, char **argv) {
    int idx, status;
    if((status = argp_parse(&args, argc, argv, 0, &idx, NULL)) != 0) {
        error(status, status, "failed to parse program arguments");
    }

    if(idx < argc) {
        diskname = argv[idx++];
    } else {
        error(EINVAL, 0, "no disk image filename provided");
    }

    if(idx < argc) {
        format = argv[idx++];
    } else {
        error(EINVAL, 0, "no disk layout format provided");
    }

    diskfd = open(diskname, O_RDWR);
    if(diskfd == -1) {
        error(errno, errno, "failed to open disk file '%s'", diskname);
    }

    if(strcmp(format, "gpt") == 0) {
        if(verbose) {
            fprintf(stderr, "formatting '%s' with GPT\n", diskname);
        }
        if(gpt_format() != 0) {
            error(errno, errno, "couldn't format disk '%s' with GPT", diskname);
        }
    } else if(strcmp(format, "mbr") == 0) {
        if(verbose) {
            fprintf(stderr, "formatting '%s' with MBR\n", diskname);
        }
        if(mbr_format() != 0) {
            error(errno, errno, "couldn't format disk '%s' with MBR", diskname);
        }
    } else {
        error(EINVAL, 0, "unknown disk layout '%s'", format);
    }

    if(close(diskfd) != 0) {
        error(errno, errno, "failed to close disk file '%s'", diskname);
    }
    return 0;
}
