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

# Set as startup project in Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT pacman_sdl)


# Calculate relative path from build dir to source dir
file(RELATIVE_PATH RELATIVE_SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

# Set output directory for executable (where resources need to be)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if(WIN32)
    # On Windows, copy files to the runtime output directory (Debug/Release folders)
    # Visual Studio creates subdirectories for each configuration
    add_custom_command(TARGET pacman_sdl POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:pacman_sdl>/levels
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/levels $<TARGET_FILE_DIR:pacman_sdl>/levels
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:pacman_sdl>/skins
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/skins $<TARGET_FILE_DIR:pacman_sdl>/skins
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:pacman_sdl>/sound
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/sound $<TARGET_FILE_DIR:pacman_sdl>/sound
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/arial.ttf $<TARGET_FILE_DIR:pacman_sdl>/arial.ttf
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/settings.conf $<TARGET_FILE_DIR:pacman_sdl>/settings.conf
        COMMENT "Copying resource files to output directory"
    )
else()
    # On Unix, create symlinks in the build directory
    file(CREATE_LINK ${RELATIVE_SOURCE_DIR}/levels ${CMAKE_CURRENT_BINARY_DIR}/levels SYMBOLIC)
    file(CREATE_LINK ${RELATIVE_SOURCE_DIR}/skins ${CMAKE_CURRENT_BINARY_DIR}/skins SYMBOLIC)
    file(CREATE_LINK ${RELATIVE_SOURCE_DIR}/sound ${CMAKE_CURRENT_BINARY_DIR}/sound SYMBOLIC)
    file(CREATE_LINK ${RELATIVE_SOURCE_DIR}/arial.ttf ${CMAKE_CURRENT_BINARY_DIR}/arial.ttf SYMBOLIC)
    file(CREATE_LINK ${RELATIVE_SOURCE_DIR}/settings.conf ${CMAKE_CURRENT_BINARY_DIR}/settings.conf SYMBOLIC)
endif()

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
)

# Link SDL libraries - use imported targets directly if available, otherwise use library paths
if(TARGET SDL2::SDL2)
    # Use imported targets directly (handles debug/release automatically)
    # On Windows, SDL2main provides the entry point wrapper
    if(WIN32 AND TARGET SDL2::SDL2main)
        target_link_libraries(pacman_sdl PRIVATE SDL2::SDL2main)
    endif()
    target_link_libraries(pacman_sdl PRIVATE 
        SDL2::SDL2
        SDL2_ttf::SDL2_ttf
        SDL2_image::SDL2_image
        SDL2_mixer::SDL2_mixer
    )
    # Check if SDL2_gfx target exists, otherwise use library path
    if(TARGET SDL2_gfx::SDL2_gfx)
        target_link_libraries(pacman_sdl PRIVATE SDL2_gfx::SDL2_gfx)
    elseif(SDL2_GFX_LIBRARY)
        target_link_libraries(pacman_sdl PRIVATE ${SDL2_GFX_LIBRARY})
    endif()
else()
    # Fallback: use library paths for non-vcpkg systems
    target_link_libraries(pacman_sdl PRIVATE 
        ${SDL2_LIBRARY}
        ${SDL2_TTF_LIBRARY}
        ${SDL2_GFX_LIBRARY}
        ${SDL2_IMAGE_LIBRARY}
        ${SDL2_MIXER_LIBRARY}
    )
endif()
