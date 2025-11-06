# Find SDL 1.2 libraries with MSYS2 and Windows support

function(find_sdl_libraries)
    # Determine MSYS2 paths if in MSYS2 environment
    set(MSYS2_INCLUDE_DIR "")
    set(MSYS2_LIB_DIR "")
    if(DEFINED ENV{MSYSTEM})
        if(EXISTS "/mingw64")
            set(MSYS2_PREFIX "/mingw64")
        elseif(EXISTS "/mingw32")
            set(MSYS2_PREFIX "/mingw32")
        elseif(EXISTS "/ucrt64")
            set(MSYS2_PREFIX "/ucrt64")
        elseif(EXISTS "/clang64")
            set(MSYS2_PREFIX "/clang64")
        else()
            set(MSYS2_PREFIX "/mingw64")
        endif()
        set(MSYS2_INCLUDE_DIR "${MSYS2_PREFIX}/include")
        set(MSYS2_LIB_DIR "${MSYS2_PREFIX}/lib")
    endif()
    
    # On Windows without MSYS2, use downloaded SDL directories
    if(WIN32 AND NOT PKG_MANAGER STREQUAL "pacman" AND DEFINED SDL_DOWNLOAD_DIR AND EXISTS ${SDL_DOWNLOAD_DIR})
        find_path(SDL_INCLUDE_DIR SDL/SDL.h
            PATHS ${SDL_DIR}/include ${SDL_DOWNLOAD_DIR}/*/include
            NO_DEFAULT_PATH
        )
        find_library(SDL_LIBRARY SDL
            PATHS ${SDL_DIR}/lib ${SDL_DOWNLOAD_DIR}/*/lib
            NO_DEFAULT_PATH
        )
        find_library(SDL_TTF_LIBRARY SDL_ttf
            PATHS ${SDL_TTF_DIR}/lib ${SDL_DOWNLOAD_DIR}/*/lib
            NO_DEFAULT_PATH
        )
        find_library(SDL_IMAGE_LIBRARY SDL_image
            PATHS ${SDL_IMAGE_DIR}/lib ${SDL_DOWNLOAD_DIR}/*/lib
            NO_DEFAULT_PATH
        )
        find_library(SDL_MIXER_LIBRARY SDL_mixer
            PATHS ${SDL_MIXER_DIR}/lib ${SDL_DOWNLOAD_DIR}/*/lib
            NO_DEFAULT_PATH
        )
    endif()
    
    # Fallback to standard find_library
    if(NOT SDL_INCLUDE_DIR)
        find_path(SDL_INCLUDE_DIR SDL/SDL.h PATHS ${MSYS2_INCLUDE_DIR})
    endif()
    if(NOT SDL_LIBRARY)
        find_library(SDL_LIBRARY SDL PATHS ${MSYS2_LIB_DIR})
    endif()
    if(NOT SDL_TTF_LIBRARY)
        find_library(SDL_TTF_LIBRARY SDL_ttf PATHS ${MSYS2_LIB_DIR})
    endif()
    if(NOT SDL_IMAGE_LIBRARY)
        find_library(SDL_IMAGE_LIBRARY SDL_image PATHS ${MSYS2_LIB_DIR})
    endif()
    if(NOT SDL_MIXER_LIBRARY)
        find_library(SDL_MIXER_LIBRARY SDL_mixer PATHS ${MSYS2_LIB_DIR})
    endif()
    if(NOT SDL_GFX_LIBRARY)
        find_library(SDL_GFX_LIBRARY SDL_gfx PATHS ${MSYS2_LIB_DIR})
    endif()
    
    if(NOT SDL_GFX_LIBRARY)
        message(WARNING "SDL_gfx not found. Install manually or build from source.")
    endif()
    
    # Set up include directories
    if(SDL_INCLUDE_DIR)
        target_include_directories(pacman_sdl PRIVATE ${SDL_INCLUDE_DIR})
        if(EXISTS "${SDL_INCLUDE_DIR}/SDL")
            target_include_directories(pacman_sdl PRIVATE "${SDL_INCLUDE_DIR}/SDL")
        endif()
    endif()
    
    # Export variables
    set(SDL_INCLUDE_DIR ${SDL_INCLUDE_DIR} PARENT_SCOPE)
    set(SDL_LIBRARY ${SDL_LIBRARY} PARENT_SCOPE)
    set(SDL_TTF_LIBRARY ${SDL_TTF_LIBRARY} PARENT_SCOPE)
    set(SDL_IMAGE_LIBRARY ${SDL_IMAGE_LIBRARY} PARENT_SCOPE)
    set(SDL_MIXER_LIBRARY ${SDL_MIXER_LIBRARY} PARENT_SCOPE)
    set(SDL_GFX_LIBRARY ${SDL_GFX_LIBRARY} PARENT_SCOPE)
endfunction()

