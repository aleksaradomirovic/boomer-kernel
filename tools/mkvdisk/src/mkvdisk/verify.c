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
    .args_doc = "DISK_NAME",
    .doc = "Verify that the contents of the disk are understandable.",
};

static bool check_gpt() {
    if(lseek(diskfd, logical_block_size * 1, SEEK_SET) < 0) {
        error(errno, errno, "failed to read disk image '%s'", diskname);
    }

    char gpt_magic[8];
    if(readf(diskfd, gpt_magic, 8) != 8) {
        error(errno, errno, "failed to read disk image '%s'", diskname);
    }

    return (strncmp(gpt_magic, "EFI PART", 8) == 0);
}

int vdisk_verify(int argc, char **argv) {
    int idx, status;
    if((status = argp_parse(&args, argc, argv, 0, &idx, NULL)) != 0) {
        error(status, status, "failed to parse program arguments");
    }

    if(idx < argc) {
        diskname = argv[idx++];
    } else {
        error(EINVAL, 0, "no disk image filename provided");
    }

    diskfd = open(diskname, O_RDONLY);
    if(diskfd == -1) {
        error(errno, errno, "failed to open disk file '%s'", diskname);
    }

    if(verbose) {
        fprintf(stderr, "deducing partition layout type\n");
    }
    
    if(check_gpt()) {
        if(verbose) {
            fprintf(stderr, "partition layout type is GPT\n");
        }
        if(gpt_verify() != 0) {
            error(errno, errno, "couldn't verify GPT disk '%s'", diskname);
        }
    } else {
        if(verbose) {
            fprintf(stderr, "partition layout type is most likely MBR\n");
        }
        if(mbr_verify() != 0) {
            error(errno, errno, "couldn't verify MBR disk '%s'", diskname);
        }
    }

    fprintf(stderr, "disk '%s' is valid\n", diskname);

    if(close(diskfd) != 0) {
        error(errno, errno, "failed to close disk file '%s'", diskname);
    }
    return 0;
}
