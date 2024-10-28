#pragma once

namespace rr {

/**
 * @brief Version information for the RoundRobin library
 */
struct Version {
    static constexpr int major = 0;
    static constexpr int minor = 1;
    static constexpr int patch = 0;
    
    /**
     * @brief Get version string in format "MAJOR.MINOR.PATCH"
     */
    static constexpr const char* string = "0.1.0";
    
    /**
     * @brief Get git commit hash if built from git, empty string otherwise
     */
    #ifdef ROUND_ROBIN_GIT_HASH
    static constexpr const char* git_hash = ROUND_ROBIN_GIT_HASH;
    #else
    static constexpr const char* git_hash = "";
    #endif
};

} // namespace rr
