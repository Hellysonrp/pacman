/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#pragma once

#include <boost/shared_ptr.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_rotozoom.h>
#include <string>

using boost::shared_ptr;

class Object
{
public:
    Object(SDL_Renderer* renderer, int os )
    :	renderer(renderer),
            offset(os),
            paused(true),
            alpha(255)
    {
    }

    virtual ~Object() {}

    void setPaused(bool b) { paused = b; }
    void setAlpha(int a) { alpha = a; }

    virtual void reset( int ix, int iy) = 0;
    virtual void Update(int time) = 0;

    virtual void Draw() = 0;
    virtual void Draw(int ix, int iy, int obj=3, int type=1)=0;
    virtual bool LoadTextures(std::string path)=0;

    virtual shared_ptr<SDL_Surface> Rotate( shared_ptr<SDL_Surface> src, int angle, double zoomx=1, double zoomy=1 ) {
        SDL_PixelFormat *fmt;

        if (!(angle < 360 && angle >= 0 && angle % 90 == 0))
            return src;

        int turns = (360 - angle) / 90;

        shared_ptr<SDL_Surface> zoomedSurface(
                zoomSurface(src.get(), zoomx, zoomy, SMOOTHING_OFF),
                SDL_FreeSurface);

        /* for some reason, if we don't exit here without rotation we get an invisible surface.. */
        if (turns % 4 == 0)
            return zoomedSurface;

        shared_ptr<SDL_Surface> rotatedSurface(
                rotateSurface90Degrees(zoomedSurface.get(), turns),
                SDL_FreeSurface);

        return rotatedSurface;
    }
protected:
    SDL_Renderer* renderer;

    const int
            offset;

    bool
            paused;

    int
            alpha;
};
