# Download SDL 1.2 libraries from official website for Windows

function(download_sdl_libraries)
    set(SDL_DOWNLOAD_DIR "${CMAKE_BINARY_DIR}/sdl_downloads")
    file(MAKE_DIRECTORY ${SDL_DOWNLOAD_DIR})
    
    # SDL 1.2.15
    set(SDL_DIR "${SDL_DOWNLOAD_DIR}/SDL-1.2.15")
    if(NOT EXISTS ${SDL_DIR})
        message(STATUS "Downloading SDL 1.2.15...")
        file(DOWNLOAD 
            "https://www.libsdl.org/release/SDL-devel-1.2.15-VC.zip" 
            "${SDL_DOWNLOAD_DIR}/SDL.zip" 
            SHOW_PROGRESS
        )
        execute_process(COMMAND powershell -Command "Expand-Archive -Path ${SDL_DOWNLOAD_DIR}/SDL.zip -DestinationPath ${SDL_DOWNLOAD_DIR} -Force")
    endif()
    
    # SDL_ttf 2.0.11
    set(SDL_TTF_DIR "${SDL_DOWNLOAD_DIR}/SDL_ttf-2.0.11")
    if(NOT EXISTS ${SDL_TTF_DIR})
        message(STATUS "Downloading SDL_ttf 2.0.11...")
        file(DOWNLOAD 
            "https://www.libsdl.org/projects/SDL_ttf/release/SDL_ttf-devel-2.0.11-VC.zip" 
            "${SDL_DOWNLOAD_DIR}/SDL_ttf.zip" 
            SHOW_PROGRESS
        )
        execute_process(COMMAND powershell -Command "Expand-Archive -Path ${SDL_DOWNLOAD_DIR}/SDL_ttf.zip -DestinationPath ${SDL_DOWNLOAD_DIR} -Force")
    endif()
    
    # SDL_image 1.2.12
    set(SDL_IMAGE_DIR "${SDL_DOWNLOAD_DIR}/SDL_image-1.2.12")
    if(NOT EXISTS ${SDL_IMAGE_DIR})
        message(STATUS "Downloading SDL_image 1.2.12...")
        file(DOWNLOAD 
            "https://www.libsdl.org/projects/SDL_image/release/SDL_image-devel-1.2.12-VC.zip" 
            "${SDL_DOWNLOAD_DIR}/SDL_image.zip" 
            SHOW_PROGRESS
        )
        execute_process(COMMAND powershell -Command "Expand-Archive -Path ${SDL_DOWNLOAD_DIR}/SDL_image.zip -DestinationPath ${SDL_DOWNLOAD_DIR} -Force")
    endif()
    
    # SDL_mixer 1.2.12
    set(SDL_MIXER_DIR "${SDL_DOWNLOAD_DIR}/SDL_mixer-1.2.12")
    if(NOT EXISTS ${SDL_MIXER_DIR})
        message(STATUS "Downloading SDL_mixer 1.2.12...")
        file(DOWNLOAD 
            "https://www.libsdl.org/projects/SDL_mixer/release/SDL_mixer-devel-1.2.12-VC.zip" 
            "${SDL_DOWNLOAD_DIR}/SDL_mixer.zip" 
            SHOW_PROGRESS
        )
        execute_process(COMMAND powershell -Command "Expand-Archive -Path ${SDL_DOWNLOAD_DIR}/SDL_mixer.zip -DestinationPath ${SDL_DOWNLOAD_DIR} -Force")
    endif()
    
    message(STATUS "SDL_gfx: No official pre-built binaries available.")
    message(STATUS "SDL_gfx must be built from source or installed manually.")
    message(STATUS "Source: https://www.ferzkopp.net/Software/SDL_gfx-2.0/")
    message(WARNING "Boost: Install manually or use a package manager like Chocolatey (boost-msvc-XX)")
    
    set(SDL_DOWNLOAD_DIR ${SDL_DOWNLOAD_DIR} PARENT_SCOPE)
    set(SDL_DIR ${SDL_DIR} PARENT_SCOPE)
    set(SDL_TTF_DIR ${SDL_TTF_DIR} PARENT_SCOPE)
    set(SDL_IMAGE_DIR ${SDL_IMAGE_DIR} PARENT_SCOPE)
    set(SDL_MIXER_DIR ${SDL_MIXER_DIR} PARENT_SCOPE)
endfunction()

