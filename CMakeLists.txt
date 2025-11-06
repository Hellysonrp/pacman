cmake_minimum_required(VERSION 3.31)

project(Pacman)

# Detect platform and package manager
if(UNIX AND NOT APPLE)
    # Linux - detect distro
    find_program(APT_GET apt-get)
    find_program(YUM yum)
    find_program(DNF dnf)
    find_program(PACMAN pacman)
    find_program(DPKG dpkg)
    
    if(APT_GET AND DPKG)
        set(PKG_MANAGER apt-get)
        set(PKG_CHECK dpkg -l)
        set(PKG_INSTALL_CMD sudo ${APT_GET} install -y)
        set(PKG_UPDATE_CMD sudo ${APT_GET} update)
        set(REQUIRED_PACKAGES 
            libsdl1.2-dev 
            libsdl-ttf2.0-dev 
            libsdl-gfx1.2-dev 
            libsdl-image1.2-dev 
            libsdl-mixer1.2-dev 
            libboost-dev
        )
    elseif(DNF)
        set(PKG_MANAGER dnf)
        set(PKG_CHECK rpm -q)
        set(PKG_INSTALL_CMD sudo ${DNF} install -y)
        set(PKG_UPDATE_CMD sudo ${DNF} check-update || true)
        set(REQUIRED_PACKAGES 
            SDL-devel 
            SDL_ttf-devel 
            SDL_gfx-devel 
            SDL_image-devel 
            SDL_mixer-devel 
            boost-devel
        )
    elseif(YUM)
        set(PKG_MANAGER yum)
        set(PKG_CHECK rpm -q)
        set(PKG_INSTALL_CMD sudo ${YUM} install -y)
        set(PKG_UPDATE_CMD sudo ${YUM} check-update || true)
        set(REQUIRED_PACKAGES 
            SDL-devel 
            SDL_ttf-devel 
            SDL_gfx-devel 
            SDL_image-devel 
            SDL_mixer-devel 
            boost-devel
        )
    elseif(PACMAN)
        set(PKG_MANAGER pacman)
        set(PKG_CHECK pacman -Q)
        set(PKG_INSTALL_CMD sudo ${PACMAN} -S --noconfirm)
        set(PKG_UPDATE_CMD sudo ${PACMAN} -Sy)
        set(REQUIRED_PACKAGES 
            sdl 
            sdl_ttf 
            sdl_gfx 
            sdl_image 
            sdl_mixer 
            boost
        )
    endif()
elseif(APPLE)
    # macOS
    find_program(BREW brew)
    if(BREW)
        set(PKG_MANAGER brew)
        set(PKG_CHECK ${BREW} list)
        set(PKG_INSTALL_CMD ${BREW} install)
        set(PKG_UPDATE_CMD ${BREW} update)
        set(REQUIRED_PACKAGES 
            sdl 
            sdl_ttf 
            sdl_gfx 
            sdl_image 
            sdl_mixer 
            boost
        )
    endif()
elseif(WIN32)
    # Windows
    find_program(CHOCO choco)
    find_program(VCPKG vcpkg)
    
    if(CHOCO)
        set(PKG_MANAGER choco)
        set(PKG_CHECK ${CHOCO} list --local-only)
        set(PKG_INSTALL_CMD ${CHOCO} install -y)
        set(PKG_UPDATE_CMD echo "Chocolatey update not needed")
        set(REQUIRED_PACKAGES 
            sdl 
            sdl-ttf 
            sdl-gfx 
            sdl-image 
            sdl-mixer 
            boost
        )
    elseif(VCPKG)
        set(PKG_MANAGER vcpkg)
        set(PKG_CHECK ${VCPKG} list)
        set(PKG_INSTALL_CMD ${VCPKG} install)
        set(PKG_UPDATE_CMD echo "vcpkg update not needed")
        set(REQUIRED_PACKAGES 
            sdl1 
            sdl-ttf 
            sdl-gfx 
            sdl-image 
            sdl-mixer 
            boost
        )
    endif()
endif()

# Function to check if package is installed
function(check_package_installed PACKAGE_NAME PKG_CHECK_CMD RESULT_VAR)
    if(PKG_MANAGER STREQUAL "apt-get")
        execute_process(
            COMMAND ${PKG_CHECK_CMD} ${PACKAGE_NAME}
            OUTPUT_QUIET
            ERROR_QUIET
            RESULT_VARIABLE PKG_RESULT
        )
        set(${RESULT_VAR} ${PKG_RESULT} PARENT_SCOPE)
    elseif(PKG_MANAGER STREQUAL "pacman" OR PKG_MANAGER STREQUAL "brew")
        execute_process(
            COMMAND ${PKG_CHECK_CMD} ${PACKAGE_NAME}
            OUTPUT_QUIET
            ERROR_QUIET
            RESULT_VARIABLE PKG_RESULT
        )
        set(${RESULT_VAR} ${PKG_RESULT} PARENT_SCOPE)
    elseif(PKG_MANAGER STREQUAL "yum" OR PKG_MANAGER STREQUAL "dnf")
        execute_process(
            COMMAND ${PKG_CHECK_CMD} ${PACKAGE_NAME}
            OUTPUT_QUIET
            ERROR_QUIET
            RESULT_VARIABLE PKG_RESULT
        )
        set(${RESULT_VAR} ${PKG_RESULT} PARENT_SCOPE)
    elseif(PKG_MANAGER STREQUAL "choco")
        execute_process(
            COMMAND ${PKG_CHECK_CMD} ${PACKAGE_NAME}
            OUTPUT_QUIET
            ERROR_QUIET
            RESULT_VARIABLE PKG_RESULT
        )
        set(${RESULT_VAR} ${PKG_RESULT} PARENT_SCOPE)
    elseif(PKG_MANAGER STREQUAL "vcpkg")
        execute_process(
            COMMAND ${PKG_CHECK_CMD} ${PACKAGE_NAME}
            OUTPUT_QUIET
            ERROR_QUIET
            RESULT_VARIABLE PKG_RESULT
        )
        set(${RESULT_VAR} ${PKG_RESULT} PARENT_SCOPE)
    else()
        set(${RESULT_VAR} 1 PARENT_SCOPE)
    endif()
endfunction()

# Check and install missing packages
if(PKG_MANAGER)
    set(MISSING_PACKAGES)
    
    foreach(PKG ${REQUIRED_PACKAGES})
        check_package_installed(${PKG} "${PKG_CHECK}" PKG_RESULT)
        
        if(NOT PKG_RESULT EQUAL 0)
            list(APPEND MISSING_PACKAGES ${PKG})
            message(STATUS "Missing: ${PKG}")
        else()
            message(STATUS "Found: ${PKG}")
        endif()
    endforeach()
    
    if(MISSING_PACKAGES)
        message(STATUS "Installing missing packages: ${MISSING_PACKAGES}")
        execute_process(
            COMMAND ${PKG_UPDATE_CMD}
            RESULT_VARIABLE UPDATE_RESULT
        )
        
        set(FAILED_PACKAGES)
        foreach(PKG ${MISSING_PACKAGES})
            message(STATUS "Installing ${PKG}...")
            execute_process(
                COMMAND ${PKG_INSTALL_CMD} ${PKG}
                RESULT_VARIABLE INSTALL_RESULT
                OUTPUT_QUIET
                ERROR_QUIET
            )
            if(NOT INSTALL_RESULT EQUAL 0)
                list(APPEND FAILED_PACKAGES ${PKG})
                message(WARNING "Package ${PKG} not available in package manager. Will try to find installed version.")
            endif()
        endforeach()
        
        if(FAILED_PACKAGES)
            message(STATUS "Some packages may need manual installation: ${FAILED_PACKAGES}")
        endif()
    else()
        message(STATUS "All dependencies are installed")
    endif()
else()
    message(WARNING "Package manager not detected. Please install dependencies manually.")
endif()

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
find_package(Boost REQUIRED)

# Find SDL 1.2 libraries
find_library(SDL_LIBRARY SDL)
find_library(SDL_TTF_LIBRARY SDL_ttf)
find_library(SDL_IMAGE_LIBRARY SDL_image)
find_library(SDL_GFX_LIBRARY SDL_gfx)
find_library(SDL_MIXER_LIBRARY SDL_mixer)
find_path(SDL_INCLUDE_DIR SDL/SDL.h)

target_include_directories(pacman_sdl PRIVATE ${SDL_INCLUDE_DIR})
target_link_libraries(pacman_sdl PRIVATE 
    Boost::boost 
    ${SDL_LIBRARY} 
    ${SDL_TTF_LIBRARY} 
    ${SDL_GFX_LIBRARY} 
    ${SDL_IMAGE_LIBRARY} 
    ${SDL_MIXER_LIBRARY}
)
