cmake_minimum_required(VERSION 3.31)

# Set up vcpkg toolchain for Windows (must be before project())
if(WIN32 AND DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

project(Pacman)

# Set C++17 standard for std::filesystem
set(CMAKE_CXX_STANDARD 17)

# Add cmake modules directory
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Detect package manager
include(DetectPackageManager)
detect_package_manager()

# SDL2 libraries are installed via package manager (vcpkg on Windows)

# Install missing packages
include(InstallPackages)
install_missing_packages()

# Source files
file(GLOB_RECURSE CPP_FILES src/*.cpp)
file(GLOB_RECURSE HEADER_FILES src/*.h)
file(GLOB_RECURSE LEVEL_FILES src/levels/*)
file(GLOB_RECURSE SKIN_FILES src/skins/*)
file(GLOB_RECURSE SOUND_FILES src/sound/*)

add_executable(pacman_sdl ${CPP_FILES} ${HEADER_FILES} ${LEVEL_FILES} ${SKIN_FILES} ${SOUND_FILES})

# Calculate relative path from build dir to source dir
file(RELATIVE_PATH RELATIVE_SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

# Create relative symlinks to resource directories
file(CREATE_LINK ${RELATIVE_SOURCE_DIR}/levels ${CMAKE_CURRENT_BINARY_DIR}/levels SYMBOLIC)
file(CREATE_LINK ${RELATIVE_SOURCE_DIR}/skins ${CMAKE_CURRENT_BINARY_DIR}/skins SYMBOLIC)
file(CREATE_LINK ${RELATIVE_SOURCE_DIR}/sound ${CMAKE_CURRENT_BINARY_DIR}/sound SYMBOLIC)
file(CREATE_LINK ${RELATIVE_SOURCE_DIR}/arial.ttf ${CMAKE_CURRENT_BINARY_DIR}/arial.ttf SYMBOLIC)

source_group("Source" FILES ${CPP_FILES})
source_group("Headers" FILES ${HEADER_FILES})
source_group("Levels" FILES ${LEVEL_FILES})
source_group("Skins" FILES ${SKIN_FILES})
source_group("Sounds" FILES ${SOUND_FILES})

# Find Boost (vcpkg provides proper CMake config on Windows)
find_package(Boost REQUIRED COMPONENTS filesystem)

# Find SDL libraries
include(FindSDL)
find_sdl_libraries()

# Link libraries
target_link_libraries(pacman_sdl PRIVATE 
    Boost::boost
    Boost::filesystem
    ${SDL2_LIBRARY}
    ${SDL2_TTF_LIBRARY}
    ${SDL2_GFX_LIBRARY}
    ${SDL2_IMAGE_LIBRARY}
    ${SDL2_MIXER_LIBRARY}
)
