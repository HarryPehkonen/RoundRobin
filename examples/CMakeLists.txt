cmake_minimum_required(VERSION 3.14)

# Basic usage example
add_executable(basic_usage basic_usage.cpp)
target_link_libraries(basic_usage PRIVATE round_robin)

# Thread safety example
add_executable(thread_safety thread_safety.cpp)
target_link_libraries(thread_safety PRIVATE round_robin Threads::Threads)

# Install examples if desired
install(TARGETS basic_usage thread_safety
    RUNTIME DESTINATION examples)

# Copy source files to examples directory
install(FILES
    basic_usage.cpp
    thread_safety.cpp
    DESTINATION examples/src)
