To use this in other projects:
```cmake
# In your project's CMakeLists.txt
include(FetchContent)
FetchContent_Declare(
    round_robin
    GIT_REPOSITORY https://github.com/yourusername/round_robin.git
    GIT_TAG v1.0.0
)
FetchContent_MakeAvailable(round_robin)

target_link_libraries(your_target PRIVATE RoundRobin::round_robin)
```

Run all tests:

```bash
make test        # Basic form
ctest           # Alternative form
make check      # Verbose form (shows output even on success)
```

Run a specific test:
```bash
ctest -R basic    # Run only basic tests
ctest -R thread   # Run only thread tests
```

Run with verbose output:
```bash
ctest -V         # Show all test output
ctest --output-on-failure  # Show output only for failed tests
```