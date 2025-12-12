#pragma once

#include <filesystem>
#include <string>

namespace PathUtils {
    // Returns the directory containing the executable
    std::filesystem::path getAppPath();
    
    // Returns the user configuration directory (cross-platform)
    // Linux: ~/.config/pacman_sdl
    // Windows: %APPDATA%/pacman_sdl
    // macOS: ~/Library/Application Support/pacman_sdl
    std::filesystem::path getHomeConfigPath();
}


