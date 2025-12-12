/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "App.h"
#include "Sounds.h"

extern Log logtxt;
extern Settings settings;

void App::InitWindow() {
    try {
        if ( !settings.fieldwidth || !settings.fieldheight || !settings.tilesize ) {
            logtxt.print("fieldheight/fieldwidth/tilesize is not set, reverting to default window dimensions");
            std::cerr << "fieldheight/fieldwidth/tilesize is not set, reverting to default window dimensions";
        }
        else {
            settings.height=settings.fieldheight*settings.tilesize;
            settings.width=settings.fieldwidth*settings.tilesize;
        }

        window = SDL_CreateWindow("Pacman", 
                                 SDL_WINDOWPOS_UNDEFINED, 
                                 SDL_WINDOWPOS_UNDEFINED,
                                 settings.width, 
                                 settings.height+EXTRA_Y_SPACE,
                                 SDL_WINDOW_BORDERLESS | SDL_WINDOW_SHOWN);
        if (!window)
            throw Error("Error while creating window");

        renderer = SDL_CreateRenderer(window, -1, 
                                      SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (!renderer) {
            SDL_DestroyWindow(window);
            window = nullptr;
            throw Error("Error while creating renderer");
        }

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        logtxt.print("Window and renderer created successfully");
    }
    catch ( Error& err ) {
        std::cerr << (err.getDesc() );
        setQuit(true);
        logtxt.print( err.getDesc() );
    }
    catch (...) {
        std::cerr << "Unexpected exception";
        setQuit(true);
        logtxt.print( "Unexpected exception in App::InitWindow()" );
    }
}

void App::InitApp() {

    try {

        if ( SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0 )
            throw Error("Error while initializing SDL");

        logtxt.print("SDL systems initialized");

        if ( TTF_Init() < 0 )
            throw Error("Error while initializing SDL_ttf");

        logtxt.print("SDL_ttf initialized");
    }
    catch ( Error& err ) {
        std::cerr << (err.getDesc() );
        setQuit(true);
        logtxt.print( err.getDesc() );
    }
    catch (...) {
        std::cerr << "Unexpected exception";
        setQuit(true);
        logtxt.print( "Unexpected exception in App::App()" );
    }
}

void App::InitSound() {

    try {

        snd = new Sounds();
        snd->init();

        logtxt.print("Sound initialized");
    }
    catch ( Error& err ) {
        std::cerr << (err.getDesc() );
        setQuit(true);
        logtxt.print( err.getDesc() );
    }
    catch (...) {
        std::cerr << "Unexpected exception";
        setQuit(true);
        logtxt.print( "Unexpected exception in App::InitSound()" );
    }
}

App::App()
:   quit(false),
    window(nullptr),
    renderer(nullptr),
    snd(NULL)

{
}

App::~App(void)
{
    try {
        if ( snd ) {
            delete snd;
            snd = NULL;
        }
    }
    catch ( Error& err ) {
        std::cerr << (err.getDesc() );
        setQuit(true);
        logtxt.print( err.getDesc() );
    }
    catch (...) {
        std::cerr << "Unexpected exception";
        setQuit(true);
        logtxt.print( "Unexpected exception in App::~App()" );
    }
}
void App::PrepareShutdown() {

    if ( TTF_WasInit() )
        TTF_Quit();

    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    SDL_Quit();

}
