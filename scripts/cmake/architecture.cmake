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

if(NOT DEFINED CPU)
    set(CPU ${CMAKE_HOST_SYSTEM_PROCESSOR} CACHE STRING "CPU" FORCE)
    message(WARNING "CPU not specified, defaulting to: ${CPU}")
endif()

if(CPU STREQUAL i686)
    set(TESTING_EMULATOR qemu-i386)
elseif(CPU STREQUAL x86_64)
else()
    message(FATAL_ERROR "CPU architecture not supported: ${CPU}")
endif()

if(NOT DEFINED KERNEL_TARGET)
    set(KERNEL_TARGET ${CPU}-elf)
endif()

if(NOT DEFINED TESTING_TARGET)
    set(TESTING_TARGET ${CPU}-linux-gnu)
endif()

if(NOT DEFINED TESTING_EMULATOR)
    set(TESTING_EMULATOR qemu-${CPU})
endif()
