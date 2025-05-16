# Copyright (C) 2025  Aleksa Radomirovic
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

include_directories(include)

add_executable(
    mkvfs
        src/mkvfs.c
)

add_executable(
    mkvdisk
        src/disk/gpt.c
        src/disk/mbr.c

        src/mkvdisk/create.c
        src/mkvdisk/format.c
        src/mkvdisk/verify.c
        src/mkvdisk.c
)
add_dependencies(mkvdisk mkvfs)
