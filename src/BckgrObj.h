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
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <string>
#include <iostream>
#include "Object.h"
#include "Log.h"
#include "App.h"
#include "Game.h"
#include "Settings.h"
#include "Error.h"

using boost::shared_ptr;

// BckgrObj constants
constexpr int NUM_OF_MAP_TEXTURES = 10;
constexpr int TILE_SIZE = 20;
constexpr int TILE_OFFSET = 10;
constexpr int ALPHA_OPAQUE = 255;

// Map tile types
enum MapTileType {
    EMPTY = 0,
    HORIZONTAL_WALL = 1,
    GHOST_DOOR = 2,
    UPPER_LEFT_CORNER = 3,
    UPPER_RIGHT_CORNER = 4,
    LOWER_RIGHT_CORNER = 5,
    LOWER_LEFT_CORNER = 6,
    LEFT_T = 7,
    RIGHT_T = 8,
    STUB = 9
};

// Object map types
enum ObjectMapType {
    OBJ_EMPTY = 0,
    SMALL_DOT = 1,
    LARGE_DOT = 2,
    FRUIT = 3
};

class BckgrObj :
	public Object
{
public:
    BckgrObj(shared_ptr<SDL_Surface> buffer, int os);

    void Draw();
    void Draw(int ix, int iy, int obj=3, int type=1);
    void Draw(int ix, int iy, int obj, int type, int alp);

    virtual void reset( int ix, int iy) { ix = iy; };   /* avoid compiler warnings */
    virtual void Update(int time) { time = 0; };        /* avoid compiler warnings */

    bool LoadTextures(std::string path);

    void setSpecialSpawned(bool b) {specialspawned = b;	}
    void setSpecialEaten(bool b) {specialeaten = b;	}

    int getObjCount() {	return objcounter;	}

    void setFruitAlpha(int a);

private:
    shared_ptr<SDL_Surface>
            mapEl[NUM_OF_MAP_TEXTURES],
            objEl[NUM_OF_MAP_TEXTURES],
            mapElRot[NUM_OF_MAP_TEXTURES][3];

    int
            objcounter,
            fruitalpha;

    bool
            specialspawned,
            specialeaten;

    void drawSprite(shared_ptr<SDL_Surface> sprite, SDL_Rect& position, int alpha);
};
