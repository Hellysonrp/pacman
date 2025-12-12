cmake_minimum_required(VERSION 3.31)

project(Pacman)

# Set C++17 standard for std::filesystem
set(CMAKE_CXX_STANDARD 17)

# Add cmake modules directory
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Detect package manager
include(DetectPackageManager)
detect_package_manager()

# Handle Windows SDL download if needed
if(WIN32 AND NOT PKG_MANAGER STREQUAL "pacman")
    include(DownloadSDL)
    download_sdl_libraries()
endif()

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
file(CREATE_LINK ${RELATIVE_SOURCE_DIR}/PAC-FONT.ttf ${CMAKE_CURRENT_BINARY_DIR}/PAC-FONT.ttf SYMBOLIC)

source_group("Source" FILES ${CPP_FILES})
source_group("Headers" FILES ${HEADER_FILES})
source_group("Levels" FILES ${LEVEL_FILES})
source_group("Skins" FILES ${SKIN_FILES})
source_group("Sounds" FILES ${SOUND_FILES})

# Find Boost
if(DEFINED ENV{MSYSTEM})
    if(EXISTS "/mingw64")
        set(BOOST_ROOT "/mingw64")
    elseif(EXISTS "/mingw32")
        set(BOOST_ROOT "/mingw32")
    elseif(EXISTS "/ucrt64")
        set(BOOST_ROOT "/ucrt64")
    elseif(EXISTS "/clang64")
        set(BOOST_ROOT "/clang64")
    endif()
endif()
find_package(Boost REQUIRED COMPONENTS filesystem)

# Find SDL libraries
include(FindSDL)
find_sdl_libraries()

# Link libraries
target_link_libraries(pacman_sdl PRIVATE 
    Boost::boost
    Boost::filesystem
    ${SDL_LIBRARY}
    ${SDL_TTF_LIBRARY}
    ${SDL_GFX_LIBRARY}
    ${SDL_IMAGE_LIBRARY}
    ${SDL_MIXER_LIBRARY}
)
