include(CMakeFindDependencyMacro)

# Find dependencies
find_dependency(Threads)

# Include the targets file
include("${CMAKE_CURRENT_LIST_DIR}/RoundRobinTargets.cmake")
