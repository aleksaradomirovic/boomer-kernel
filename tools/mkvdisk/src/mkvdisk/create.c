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
#include <fcntl.h>
#include <stdckdint.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

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
    .args_doc = "DISK_NAME DISK_SIZE",
    .doc = "Creates a raw disk image of a certain size.",
};

static off_t disksize = -1;

int vdisk_create(int argc, char **argv) {
    int idx, status;
    if((status = argp_parse(&args, argc, argv, 0, &idx, NULL)) != 0) {
        error(status, status, "failed to parse program arguments");
    }

    {
        if(idx < argc) {
            diskname = argv[idx++];
        } else {
            error(EINVAL, 0, "no disk image filename provided");
        }

        if(idx < argc) {
            char *sizenum = argv[idx++];
            errno = 0;

            long long result = strtoll(sizenum, &sizenum, 10);
            switch(*sizenum) {
                case '\0':
                    break;
                case 'g':
                case 'G':
                    if(ckd_mul(&result, result, 1024)) {
                        error(ERANGE, 0, "disk image size too large");
                    }
                case 'm':
                case 'M':
                    if(ckd_mul(&result, result, 1024)) {
                        error(ERANGE, 0, "disk image size too large");
                    }
                case 'k':
                case 'K':
                    if(ckd_mul(&result, result, 1024)) {
                        error(ERANGE, 0, "disk image size too large");
                    }
                    break;
                default:
                    error(EINVAL, 0, "invalid size metric '%c'", *sizenum);
                    break;
            }

            if(errno == ERANGE || ckd_mul(&disksize, result, 1)) {
                error(ERANGE, 0, "disk image size too large");
            } else if(result < 0) {
                error(EINVAL, 0, "disk image size must be a positive number");
            }
        } else {
            error(EINVAL, 0, "disk image size not provided");
        }
    }

    if(disksize % logical_block_size != 0) {
        error(EINVAL, 0, "disk size (%jd) is not a multiple of logical block size (%jd)", (intmax_t) disksize, (intmax_t) logical_block_size);
    }

    diskfd = open(diskname, O_CREAT | O_WRONLY, 00666);
    if(diskfd == -1) {
        error(errno, errno, "failed to open disk file '%s'", diskname);
    }
    
    if(verbose) {
        fprintf(stderr, "truncating %s to %jd bytes\n", diskname, (intmax_t) disksize);
    }

    if(ftruncate(diskfd, disksize) != 0) {
        error(errno, errno, "failed to truncate disk file '%s' to size %jd", diskname, (intmax_t) disksize);
    }

    if(close(diskfd) != 0) {
        error(errno, errno, "failed to close disk file '%s'", diskname);
    }
    return 0;
}
