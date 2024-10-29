cmake_minimum_required(VERSION 3.14)
project(round_robin VERSION 0.1.0 LANGUAGES CXX)

# Require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create interface library (header-only)
add_library(round_robin INTERFACE)
add_library(RoundRobin::round_robin ALIAS round_robin) # Add an alias with namespace

target_include_directories(round_robin INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Installation
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/RoundRobinConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(TARGETS round_robin
    EXPORT RoundRobinTargets
    INCLUDES DESTINATION include
)

install(EXPORT RoundRobinTargets
    FILE RoundRobinTargets.cmake
    NAMESPACE RoundRobin::
    DESTINATION lib/cmake/RoundRobin
)

install(DIRECTORY include/round_robin
    DESTINATION include
)

install(FILES
    cmake/RoundRobinConfig.cmake
    "${PROJECT_BINARY_DIR}/RoundRobinConfigVersion.cmake"
    DESTINATION lib/cmake/RoundRobin
)


# Enable AddressSanitizer (ASAN) by default
option(ENABLE_ASAN "Enable AddressSanitizer" ON)

# Enable code coverage by default
option(ENABLE_COVERAGE "Enable code coverage" ON)

if(ENABLE_ASAN)
    set(MEMORYCHECK_COMMAND "/usr/bin/valgrind")
    set(MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full --errors-for-leak-kinds=definite,indirect --error-exitcode=1")
    set(MEMORYCHECK_SUPPRESSIONS_FILE "${CMAKE_SOURCE_DIR}/tests/valgrind.supp")
endif()

# Add a custom target for generating the coverage report
add_custom_target(coverage
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/coverage_report
    COMMAND lcov --capture --directory ${CMAKE_BINARY_DIR}/tests --output-file ${CMAKE_BINARY_DIR}/coverage.info
    COMMAND lcov --remove ${CMAKE_BINARY_DIR}/coverage.info '/usr/*' --output-file ${CMAKE_BINARY_DIR}/coverage.info
    COMMAND genhtml -o ${CMAKE_BINARY_DIR}/coverage_report ${CMAKE_BINARY_DIR}/coverage.info
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS ${TEST_TARGETS}
    USES_TERMINAL
    COMMENT "Generating coverage report"
)

# Testing
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
    if(BUILD_TESTING)
        add_subdirectory(tests)
    endif()
endif()

