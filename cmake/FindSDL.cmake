# Find SDL2 libraries with vcpkg support

function(find_sdl_libraries)
    # Try to use find_package for SDL2 components (works with vcpkg or if libraries provide CMake configs)
    find_package(SDL2 QUIET)
    find_package(SDL2_ttf QUIET)
    find_package(SDL2_image QUIET)
    find_package(SDL2_mixer QUIET)
    find_package(SDL2_gfx QUIET)
    
    # If find_package worked, use the imported targets
    if(TARGET SDL2::SDL2)
        # vcpkg provides proper CMake targets - use them directly
        set(SDL2_FOUND TRUE PARENT_SCOPE)
        set(SDL2_TTF_FOUND TRUE PARENT_SCOPE)
        set(SDL2_IMAGE_FOUND TRUE PARENT_SCOPE)
        set(SDL2_MIXER_FOUND TRUE PARENT_SCOPE)
        set(SDL2_GFX_FOUND TRUE PARENT_SCOPE)
        
        # Store target names for linking (use targets directly, not paths)
        set(SDL2_LIBRARY "SDL2::SDL2" PARENT_SCOPE)
        set(SDL2_TTF_LIBRARY "SDL2_ttf::SDL2_ttf" PARENT_SCOPE)
        set(SDL2_IMAGE_LIBRARY "SDL2_image::SDL2_image" PARENT_SCOPE)
        set(SDL2_MIXER_LIBRARY "SDL2_mixer::SDL2_mixer" PARENT_SCOPE)
        
        # Check if SDL2_gfx target was found, otherwise fall back to manual finding
        if(TARGET SDL2_gfx::SDL2_gfx)
            set(SDL2_GFX_LIBRARY "SDL2_gfx::SDL2_gfx" PARENT_SCOPE)
        else()
            find_library(SDL2_GFX_LIBRARY SDL2_gfx)
        endif()
    else()
        # Fallback to manual finding when find_package didn't work
        find_path(SDL2_INCLUDE_DIR SDL2/SDL.h)
        find_library(SDL2_LIBRARY SDL2)
        find_library(SDL2_TTF_LIBRARY SDL2_ttf)
        find_library(SDL2_IMAGE_LIBRARY SDL2_image)
        find_library(SDL2_MIXER_LIBRARY SDL2_mixer)
        find_library(SDL2_GFX_LIBRARY SDL2_gfx)
        
        # Set up include directories
        if(SDL2_INCLUDE_DIR)
            target_include_directories(pacman_sdl PRIVATE ${SDL2_INCLUDE_DIR})
            if(EXISTS "${SDL2_INCLUDE_DIR}/SDL2")
                target_include_directories(pacman_sdl PRIVATE "${SDL2_INCLUDE_DIR}/SDL2")
            endif()
        endif()
    endif()
    
    # Export variables
    if(DEFINED SDL2_INCLUDE_DIR)
        set(SDL2_INCLUDE_DIR ${SDL2_INCLUDE_DIR} PARENT_SCOPE)
    endif()
    set(SDL2_LIBRARY ${SDL2_LIBRARY} PARENT_SCOPE)
    set(SDL2_TTF_LIBRARY ${SDL2_TTF_LIBRARY} PARENT_SCOPE)
    set(SDL2_IMAGE_LIBRARY ${SDL2_IMAGE_LIBRARY} PARENT_SCOPE)
    set(SDL2_MIXER_LIBRARY ${SDL2_MIXER_LIBRARY} PARENT_SCOPE)
    set(SDL2_GFX_LIBRARY ${SDL2_GFX_LIBRARY} PARENT_SCOPE)
endfunction()
