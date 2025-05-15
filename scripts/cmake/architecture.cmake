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

function(add_kernel_build_defines)
    string(REGEX REPLACE "([^;]+)" "__\\1__" NEWDEFINES "${ARGN}")
    list(APPEND KERNEL_DEFINES ${NEWDEFINES})
    return(PROPAGATE KERNEL_DEFINES)
endfunction()

function(build_param PARAM_NAME PARAM_TYPE)
    cmake_parse_arguments(
        PARAM
        "WARN"
        "DEFAULT;HINT"
        ""
        ${ARGN}
    )

    if(NOT DEFINED CACHE{${PARAM_NAME}})
        if(NOT PARAM_DEFAULT)
            set(PARAM_HINT OFF)
        endif()

        if(NOT PARAM_HINT)
            set(PARAM_HINT "No hint provided")
        endif()

        set(${PARAM_NAME} ${PARAM_DEFAULT} CACHE ${PARAM_TYPE} "${PARAM_HINT}" FORCE)

        if(PARAM_WARN)
            message(WARNING "Option ${PARAM_NAME} not specified, defaulting to: ${${PARAM_NAME}}")
        endif()
    endif()
endfunction()

function(force_build_param PARAM_NAME PARAM_TYPE PARAM_VALUE)
    if(NOT DEFINED ${PARAM_NAME})
        build_param(${PARAM_NAME} ${PARAM_TYPE} DEFAULT "${PARAM_VALUE}")
    elseif(NOT "${${PARAM_NAME}}" STREQUAL "${PARAM_VALUE}")
        message(FATAL_ERROR "Option ${PARAM_NAME} has illegal value (${${PARAM_NAME}}) for this configuration")
    endif()
endfunction()

build_param(CPU STRING DEFAULT ${CMAKE_HOST_SYSTEM_PROCESSOR} WARN)

if(CPU STREQUAL i686)
    add_kernel_build_defines(i686 x86_32 x86_all)
    set(TESTING_EMULATOR qemu-i386)

    build_param(PAGING BOOL DEFAULT ON)
    if(PAGING)
        build_param(PAE BOOL DEFAULT ON)
    endif()
    if(PAE)
        add_kernel_build_defines(pae)
    endif()
elseif(CPU STREQUAL x86_64)
    add_kernel_build_defines(x86_64 x86_all)

    force_build_param(PAGING BOOL ON)

    build_param(PML5 BOOL DEFAULT ON)
    if(PML5)
        add_kernel_build_defines(pml5)
    endif()
else()
    message(FATAL_ERROR "CPU architecture not supported: ${CPU}")
endif()

build_param(PAGING BOOL DEFAULT ON)

if(PAGING)
    add_kernel_build_defines(paging)
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
