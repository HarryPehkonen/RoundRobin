cmake_minimum_required(VERSION 3.14)
project(round_robin VERSION 1.0.0 LANGUAGES CXX)

# Require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable AddressSanitizer (ASAN) by default
option(ENABLE_ASAN "Enable AddressSanitizer" ON)

# Enable code coverage by default
option(ENABLE_COVERAGE "Enable code coverage" ON)

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

# Create interface library (header-only)
add_library(round_robin INTERFACE)
target_include_directories(round_robin INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Testing
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
    if(BUILD_TESTING)
        add_subdirectory(tests)
    endif()
endif()

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
