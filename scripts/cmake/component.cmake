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

add_library(
    kernel_common
    INTERFACE
)

add_custom_target(test)

function(get_parent_component)
    set(PARENT_COMPONENT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    while(TRUE)
        get_property(
            PARENT_COMPONENT_DIRECTORY
            DIRECTORY ${PARENT_COMPONENT_DIRECTORY}
            PROPERTY PARENT_DIRECTORY
        )

        if(PARENT_COMPONENT_DIRECTORY STREQUAL "${CMAKE_SOURCE_DIR}/kernel")
            unset(PARENT_COMPONENT)
            unset(PARENT_COMPONENT_DIRECTORY)
            break()
        endif()

        get_property(
            PARENT_COMPONENT
            DIRECTORY ${PARENT_COMPONENT_DIRECTORY}
            PROPERTY KERNEL_COMPONENT
        )

        if(PARENT_COMPONENT)
            break()
        endif()
    endwhile()

    return(PROPAGATE PARENT_COMPONENT PARENT_COMPONENT_DIRECTORY)
endfunction()

function(kernel_component)
    cmake_parse_arguments(
        COMPONENT
        ""
        "NAME"
        "SOURCES"
        ${ARGN}
    )

    if(NOT COMPONENT_NAME)
        cmake_path(GET CMAKE_CURRENT_SOURCE_DIR FILENAME COMPONENT_NAME)
    endif()

    list(APPEND COMPONENT_SOURCES ${COMPONENT_UNPARSED_ARGUMENTS})

    get_parent_component()

    if(PARENT_COMPONENT)
        set(COMPONENT_LIBRARY ${PARENT_COMPONENT}_${COMPONENT_NAME})
    else()
        set(COMPONENT_LIBRARY ${COMPONENT_NAME})
    endif()

    target_include_directories(
        kernel_common
        INTERFACE
            include
    )

    if(COMPONENT_SOURCES)
        add_library(
            ${COMPONENT_LIBRARY}
            STATIC
                ${COMPONENT_SOURCES}
        )
        target_link_libraries(
            ${COMPONENT_LIBRARY}
            PRIVATE
                kernel_common
        )
        target_compile_options(
            ${COMPONENT_LIBRARY}
            PRIVATE
                --target=${KERNEL_TARGET}
                -ffreestanding
        )
    else()
        add_library(
            ${COMPONENT_LIBRARY}
            INTERFACE
        )
    endif()

    add_custom_target(
        ${COMPONENT_LIBRARY}_test
    )

    if(PARENT_COMPONENT)
        get_property(
            PARENT_COMPONENT_TYPE
            TARGET ${PARENT_COMPONENT}
            PROPERTY TYPE
        )

        if(PARENT_COMPONENT_TYPE STREQUAL "INTERFACE_LIBRARY")
            target_link_libraries(
                ${PARENT_COMPONENT}
                INTERFACE
                    ${COMPONENT_LIBRARY}
            )
        else()
            target_link_libraries(
                ${PARENT_COMPONENT}
                PUBLIC
                    ${COMPONENT_LIBRARY}
            )
        endif()

        add_dependencies(${PARENT_COMPONENT}_test ${COMPONENT_LIBRARY}_test)
    else()
        add_dependencies(test ${COMPONENT_LIBRARY}_test)
    endif()

    set_property(
        TARGET ${COMPONENT_LIBRARY}
        PROPERTY OUTPUT_NAME ${COMPONENT_NAME}
    )

    set_property(
        DIRECTORY
        PROPERTY KERNEL_COMPONENT ${COMPONENT_LIBRARY}
    )

    # message(STATUS ${COMPONENT_LIBRARY})
endfunction()

function(kernel_component_test TEST_NAME)
    cmake_parse_arguments(
        TEST
        ""
        ""
        "SOURCES"
        ${ARGN}
    )
    list(APPEND TEST_SOURCES ${TEST_UNPARSED_ARGUMENTS})

    get_property(
        COMPONENT_LIBRARY
        DIRECTORY
        PROPERTY KERNEL_COMPONENT
    )

    set(TEST_TARGET ${COMPONENT_LIBRARY}_test_${TEST_NAME})
    set(TEST_EXECUTABLE ${TEST_TARGET}_bin)

    add_executable(
        ${TEST_EXECUTABLE}
        EXCLUDE_FROM_ALL
            ${TEST_SOURCES}
    )
    target_link_libraries(
        ${TEST_EXECUTABLE}
        PRIVATE
            kernel_common
            ${COMPONENT_LIBRARY}
            GTest::gtest_main
    )
    target_compile_options(
        ${TEST_EXECUTABLE}
        PRIVATE
            --target=${TESTING_TARGET}
            ${C_FLAGS_TESTING}
    )
    target_link_options(
        ${TEST_EXECUTABLE}
        PRIVATE
            --target=${TESTING_TARGET}
    )
    set_property(
        TARGET ${TEST_EXECUTABLE}
        PROPERTY OUTPUT_NAME test_${TEST_NAME}
    )

    add_custom_target(
        ${TEST_TARGET}
        COMMAND ${TESTING_EMULATOR} $<TARGET_FILE:${TEST_EXECUTABLE}>
        DEPENDS ${TEST_EXECUTABLE}
        # COMMENT "Running ${COMPONENT_LIBRARY} test '${TEST_NAME}'..."
    )
    
    add_dependencies(${COMPONENT_LIBRARY}_test ${TEST_TARGET})
endfunction()
