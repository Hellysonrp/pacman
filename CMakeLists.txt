cmake_minimum_required(VERSION 3.31)

project(Pacman)

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
find_package(Boost REQUIRED)

# Find SDL libraries
include(FindSDL)
find_sdl_libraries()

# Link libraries
target_link_libraries(pacman_sdl PRIVATE 
    Boost::boost 
    ${SDL_LIBRARY} 
    ${SDL_TTF_LIBRARY} 
    ${SDL_GFX_LIBRARY} 
    ${SDL_IMAGE_LIBRARY} 
    ${SDL_MIXER_LIBRARY}
)
