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
                "libsdl1.2-dev;libsdl-ttf2.0-dev;libsdl-gfx1.2-dev;libsdl-image1.2-dev;libsdl-mixer1.2-dev;libboost-dev"
                PARENT_SCOPE
            )
        elseif(DNF)
            set(PKG_MANAGER "dnf" PARENT_SCOPE)
            set(PKG_CHECK "rpm -q" PARENT_SCOPE)
            set(PKG_INSTALL_CMD "sudo ${DNF} install -y" PARENT_SCOPE)
            set(PKG_UPDATE_CMD "sudo ${DNF} check-update || true" PARENT_SCOPE)
            # Fedora uses sdl12-compat-devel for SDL 1.2 compatibility layer
            set(REQUIRED_PACKAGES 
                "sdl12-compat-devel;SDL_ttf-devel;SDL_gfx-devel;SDL_image-devel;SDL_mixer-devel;boost-devel"
                PARENT_SCOPE
            )
        elseif(YUM)
            set(PKG_MANAGER "yum" PARENT_SCOPE)
            set(PKG_CHECK "rpm -q" PARENT_SCOPE)
            set(PKG_INSTALL_CMD "sudo ${YUM} install -y" PARENT_SCOPE)
            set(PKG_UPDATE_CMD "sudo ${YUM} check-update || true" PARENT_SCOPE)
            # Note: SDL 1.2 may not be available in modern RHEL/CentOS - may need EPEL or manual install
            set(REQUIRED_PACKAGES 
                "SDL-devel;SDL_ttf-devel;SDL_gfx-devel;SDL_image-devel;SDL_mixer-devel;boost-devel"
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
                    "mingw-w64-x86_64-SDL;mingw-w64-x86_64-SDL_ttf;mingw-w64-x86_64-SDL_gfx;mingw-w64-x86_64-SDL_image;mingw-w64-x86_64-SDL_mixer;mingw-w64-x86_64-boost"
                    PARENT_SCOPE
                )
            else()
                # Arch Linux
                # SDL 1.2 packages are available in official Extra repository
                # sdl12-compat provides SDL 1.2 compatibility layer using SDL 2.0
                set(PKG_MANAGER "pacman" PARENT_SCOPE)
                set(PKG_CHECK "pacman -Q" PARENT_SCOPE)
                set(PKG_INSTALL_CMD "sudo ${PACMAN} -S --noconfirm" PARENT_SCOPE)
                set(PKG_UPDATE_CMD "sudo ${PACMAN} -Sy" PARENT_SCOPE)
                set(REQUIRED_PACKAGES 
                    "sdl12-compat;sdl_ttf;sdl_gfx;sdl_image;sdl_mixer;boost"
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
                "sdl;sdl_ttf;sdl_gfx;sdl_image;sdl_mixer;boost"
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
                "mingw-w64-x86_64-SDL;mingw-w64-x86_64-SDL_ttf;mingw-w64-x86_64-SDL_gfx;mingw-w64-x86_64-SDL_image;mingw-w64-x86_64-SDL_mixer;mingw-w64-x86_64-boost"
                PARENT_SCOPE
            )
        endif()
    endif()
endfunction()

