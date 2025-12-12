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
            # Check if this is MSYS2 (Windows) or Arch Linux
            if(DEFINED ENV{MSYSTEM})
                # MSYS2 environment
                set(PKG_MANAGER "pacman" PARENT_SCOPE)
                set(PKG_CHECK "${PACMAN} -Q" PARENT_SCOPE)
                set(PKG_INSTALL_CMD "${PACMAN} -S --noconfirm" PARENT_SCOPE)
                set(PKG_UPDATE_CMD "${PACMAN} -Sy" PARENT_SCOPE)
                set(REQUIRED_PACKAGES 
                    "mingw-w64-x86_64-SDL2;mingw-w64-x86_64-SDL2_ttf;mingw-w64-x86_64-SDL2_gfx;mingw-w64-x86_64-SDL2_image;mingw-w64-x86_64-SDL2_mixer;mingw-w64-x86_64-boost"
                    PARENT_SCOPE
                )
            else()
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
        find_program(PACMAN pacman)
        if(PACMAN AND DEFINED ENV{MSYSTEM})
            # MSYS2 on Windows
            set(PKG_MANAGER "pacman" PARENT_SCOPE)
            set(PKG_CHECK "${PACMAN} -Q" PARENT_SCOPE)
            set(PKG_INSTALL_CMD "${PACMAN} -S --noconfirm" PARENT_SCOPE)
            set(PKG_UPDATE_CMD "${PACMAN} -Sy" PARENT_SCOPE)
            set(REQUIRED_PACKAGES 
                "mingw-w64-x86_64-SDL2;mingw-w64-x86_64-SDL2_ttf;mingw-w64-x86_64-SDL2_gfx;mingw-w64-x86_64-SDL2_image;mingw-w64-x86_64-SDL2_mixer;mingw-w64-x86_64-boost"
                PARENT_SCOPE
            )
        endif()
    endif()
endfunction()

