/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include "Log.h"
#include "Settings.h"
#include "Error.h"
#include "Constants.h"
#include <boost/shared_ptr.hpp>

using boost::shared_ptr;

class Sounds;

class App
{
public:
    App();
    ~App();             //SDL systems are shut down in destructor

    void InitApp();     //SDL systems are initialized
    void InitWindow();  //creates window with parameters from loaded settings file
    void InitSound();
    void PrepareShutdown();

    //////////////
    //Global
    //////////////

    inline bool getQuit() const { return quit; }
    void setQuit( bool q ) { quit = q; }
    SDL_Renderer* getRenderer() const { return renderer; }
    SDL_Window* getWindow() const { return window; }
    Sounds* getSnd() { return snd; }

private:

    //////////////
    //Global
    //////////////

    bool
            quit;       //if true then quit application

    SDL_Window* window;
    SDL_Renderer* renderer;

    Sounds
            *snd;

};
