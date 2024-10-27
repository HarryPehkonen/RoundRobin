# Download and configure Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0  # Specify a version for reproducibility
)
# Prevent overriding parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Enable testing for this directory
enable_testing()

# Find required packages
find_package(Threads REQUIRED)

# Basic functionality tests
add_executable(basic_tests basic_tests.cpp)
target_link_libraries(basic_tests
    PRIVATE
        round_robin
        GTest::gtest
        GTest::gtest_main
)

# Thread safety tests
add_executable(thread_tests thread_tests.cpp)
target_link_libraries(thread_tests
    PRIVATE
        round_robin
        GTest::gtest
        GTest::gtest_main
        Threads::Threads
)


# Memory leak tests
add_executable(memory_leak_tests memory_leak_tests.cpp)
target_link_libraries(memory_leak_tests
    PRIVATE
        round_robin
        GTest::gtest
        GTest::gtest_main
        Threads::Threads
)

# Register tests with CTest
add_test(NAME basic_tests COMMAND basic_tests)
add_test(NAME thread_tests COMMAND thread_tests)
add_test(NAME memory_leak_tests COMMAND memory_leak_tests)

# Optional: Add custom test targets for convenience
add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS 
        basic_tests 
        thread_tests 
        memory_leak_tests
        coverage
)

# Optional: Configure test timeouts
set_tests_properties(basic_tests PROPERTIES TIMEOUT 10)
set_tests_properties(thread_tests PROPERTIES TIMEOUT 30)
set_tests_properties(memory_leak_tests PROPERTIES TIMEOUT 30)

# Optional: Add coverage flags if building for coverage
if(ENABLE_COVERAGE)
    foreach(test_target
        basic_tests
        thread_tests
        memory_leak_tests
    )
        target_compile_options(${test_target} PRIVATE --coverage)
        target_link_options(${test_target} PRIVATE --coverage)
    endforeach()
endif()

# Optional: Enable AddressSanitizer (ASan) for memory leak detection
if(ENABLE_ASAN)
    foreach(test_target
        basic_tests
        thread_tests
        memory_leak_tests
    )
        # Compiler flags for ASan
        target_compile_options(${test_target} PRIVATE
            -fsanitize=address
            -fsanitize=undefined
            -fno-omit-frame-pointer
            -g  # Include debug symbols for better reports
        )
        
        # Linker flags for ASan
        target_link_options(${test_target} PRIVATE
            -fsanitize=address
            -fsanitize=undefined
        )
        
        # Optional but recommended: Suppress expected memory leaks in 3rd party libraries
        # target_compile_definitions(${test_target} PRIVATE ASAN_SUPPRESS_MEMORY_LEAKS=1)
    endforeach()
    
    message(STATUS "AddressSanitizer (ASan) enabled for memory leak detection.")
endif()
