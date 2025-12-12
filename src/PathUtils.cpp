#include "PathUtils.h"
#include <boost/dll.hpp>
#include <cstdlib>
#include <filesystem>

namespace PathUtils {
    std::filesystem::path getAppPath() {
        return std::filesystem::path(boost::dll::program_location().parent_path().string());
    }
    
    std::filesystem::path getHomeConfigPath() {
        const char* home = std::getenv("HOME");
        
#ifdef _WIN32
        if (!home) {
            home = std::getenv("APPDATA");
        }
        if (home) {
            return std::filesystem::path(home) / "pacman_sdl";
        }
#elif defined(__APPLE__)
        if (home) {
            return std::filesystem::path(home) / "Library" / "Application Support" / "pacman_sdl";
        }
#else
        // Linux and other Unix-like systems
        if (home) {
            return std::filesystem::path(home) / ".config" / "pacman_sdl";
        }
#endif
        
        return std::filesystem::path();
    }
}

