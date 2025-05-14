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

cmake_minimum_required(VERSION 3.25)
project(kernel LANGUAGES C CXX ASM)

include(scripts/cmake/architecture.cmake)

include(FetchContent)

function(get_googletest)
    set(CMAKE_CXX_FLAGS "-Wall -O3 --target=${TESTING_TARGET}")

    FetchContent_Declare(
        googletest
        EXCLUDE_FROM_ALL
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.17.0
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endfunction()

get_googletest()

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD ${CMAKE_C_STANDARD})
set(CMAKE_C_FLAGS "-Wall -Os -fpic")
set(CMAKE_CXX_FLAGS ${CMAKE_C_FLAGS})

add_subdirectory(kernel)
