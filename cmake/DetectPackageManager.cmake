# Detect package manager and set up package installation variables

function(detect_package_manager)
    if(UNIX AND NOT APPLE)
        # Linux - detect distro
        find_program(APT_GET apt-get)
        find_program(YUM yum)
        find_program(DNF dnf)
        find_program(PACMAN pacman)
        find_program(DPKG dpkg)
        
        if(APT_GET AND DPKG)
            set(PKG_MANAGER "apt-get" PARENT_SCOPE)
            set(PKG_CHECK "dpkg -l" PARENT_SCOPE)
            set(PKG_INSTALL_CMD "sudo ${APT_GET} install -y" PARENT_SCOPE)
            set(PKG_UPDATE_CMD "sudo ${APT_GET} update" PARENT_SCOPE)
            set(REQUIRED_PACKAGES 
                "libsdl2-dev;libsdl2-ttf-dev;libsdl2-gfx-dev;libsdl2-image-dev;libsdl2-mixer-dev;libboost-dev;libboost-filesystem-dev"
                PARENT_SCOPE
            )
        elseif(DNF)
            set(PKG_MANAGER "dnf" PARENT_SCOPE)
            set(PKG_CHECK "rpm -q" PARENT_SCOPE)
            set(PKG_INSTALL_CMD "sudo ${DNF} install -y" PARENT_SCOPE)
            set(PKG_UPDATE_CMD "sudo ${DNF} check-update || true" PARENT_SCOPE)
            # Fedora SDL2 packages
            set(REQUIRED_PACKAGES 
                "SDL2-devel;SDL2_ttf-devel;SDL2_gfx-devel;SDL2_image-devel;SDL2_mixer-devel;boost-devel"
                PARENT_SCOPE
            )
        elseif(YUM)
            set(PKG_MANAGER "yum" PARENT_SCOPE)
            set(PKG_CHECK "rpm -q" PARENT_SCOPE)
            set(PKG_INSTALL_CMD "sudo ${YUM} install -y" PARENT_SCOPE)
            set(PKG_UPDATE_CMD "sudo ${YUM} check-update || true" PARENT_SCOPE)
            # RHEL/CentOS SDL2 packages
            set(REQUIRED_PACKAGES 
                "SDL2-devel;SDL2_ttf-devel;SDL2_gfx-devel;SDL2_image-devel;SDL2_mixer-devel;boost-devel"
                PARENT_SCOPE
            )
        elseif(PACMAN)
            # Arch Linux
            # SDL2 packages are available in official Extra repository
            set(PKG_MANAGER "pacman" PARENT_SCOPE)
            set(PKG_CHECK "pacman -Q" PARENT_SCOPE)
            set(PKG_INSTALL_CMD "sudo ${PACMAN} -S --noconfirm" PARENT_SCOPE)
            set(PKG_UPDATE_CMD "sudo ${PACMAN} -Sy" PARENT_SCOPE)
            set(REQUIRED_PACKAGES 
                "sdl2;sdl2_ttf;sdl2_gfx;sdl2_image;sdl2_mixer;boost"
                PARENT_SCOPE
            )
        endif()
    elseif(APPLE)
        find_program(BREW brew)
        if(BREW)
            set(PKG_MANAGER "brew" PARENT_SCOPE)
            set(PKG_CHECK "${BREW} list" PARENT_SCOPE)
            set(PKG_INSTALL_CMD "${BREW} install" PARENT_SCOPE)
            set(PKG_UPDATE_CMD "${BREW} update" PARENT_SCOPE)
            set(REQUIRED_PACKAGES 
                "sdl2;sdl2_ttf;sdl2_gfx;sdl2_image;sdl2_mixer;boost"
                PARENT_SCOPE
            )
        endif()
    elseif(WIN32)
        # Windows - use vcpkg
        if(DEFINED ENV{VCPKG_ROOT})
            set(VCPKG_ROOT "$ENV{VCPKG_ROOT}" PARENT_SCOPE)
            find_program(VCPKG vcpkg PATHS "$ENV{VCPKG_ROOT}" NO_DEFAULT_PATH)
            if(NOT VCPKG)
                find_program(VCPKG vcpkg)
            endif()
            
            if(VCPKG)
                set(PKG_MANAGER "vcpkg" PARENT_SCOPE)
                set(VCPKG_EXE ${VCPKG} PARENT_SCOPE)
                set(PKG_CHECK "${VCPKG} list" PARENT_SCOPE)
                set(PKG_INSTALL_CMD "${VCPKG} install" PARENT_SCOPE)
                set(PKG_UPDATE_CMD "echo No update needed for vcpkg" PARENT_SCOPE)
                set(REQUIRED_PACKAGES 
                    "sdl2;sdl2-ttf;sdl2-gfx;sdl2-image;sdl2-mixer;boost-filesystem;boost-dll"
                    PARENT_SCOPE
                )
            else()
                message(FATAL_ERROR 
                    "vcpkg not found. Please install vcpkg and set the VCPKG_ROOT environment variable.\n"
                    "See https://github.com/Microsoft/vcpkg for installation instructions."
                )
            endif()
        else()
            message(FATAL_ERROR 
                "VCPKG_ROOT environment variable is not set.\n"
                "Please install vcpkg and set the VCPKG_ROOT environment variable to the vcpkg installation directory.\n"
                "See https://github.com/Microsoft/vcpkg for installation instructions."
            )
        endif()
    endif()
endfunction()

