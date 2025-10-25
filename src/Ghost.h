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
#include <SDL/SDL_rotozoom.h>
#include <string>
#include <iostream>
#include <cstdlib>

#include "Constants.h"
#include "Object.h"
#include "Position.h"
#include "BHeap.h"
#include "Log.h"
#include "App.h"
#include "Settings.h"
#include "Error.h"

using boost::shared_ptr;

class Ghost :
	public Object
{
public:
    Ghost(shared_ptr<SDL_Surface> buf, int os, int ix, int iy, int ispdmod, int itilesize,
            int iheight, int iwidth, int *imap, std::string fn);
    ~Ghost();

    void Draw();
    void Draw(int ix, int iy, int obj=0, int type=0) ;

    void Update( int time );
    bool LoadTextures(std::string path);

    void setState(int st);
    int getState();
    void setTarget(int ix, int iy);
    bool find();
    bool collision_ignore_gate(int xtmp, int ytmp);
    bool collision(int xtmp, int ytmp);
    void pathCalcNormal();
    void pathCalcDead();
    void pathCalcVuln();
    bool tracePath();
    int calcF(int ix, int iy) const;
    void reset(int ix, int iy);
    void changeDifficulty(int spd, int iq);

    int getXpix();
    int getYpix();
private:
    Position
            tilePos,	//current position in tiles (all positions refer to upper left corner)
            pixelPos,	//current position in pixels
            direction,	//current direction in x and y
            nextDirection,	//queued direction
            target,		//target position
            pixelPosAtLastDirChange; //location where the last direction change has taken place

    int
            spdmod,	// speed modifier	- default 100?

            tilesize,
            height,
            width,

            *map,

            defspeed,	//default speed

            dirToTar,	//stores direction to target

            *parentDir,		// direction to parent tile. -1 == parent dir, 4 == init state
            *Gstore,	// G value of each square. -1 == init state
            *closedF,	// F value of each closed square. -1 == init state

            state,		// 0 == normal, 1 == vulnerable, 2 == warning, 3 == dead

            animcounter,
            baddie_iq,		// increases the chance of choosing shortest path

            baddie_start_point_x,
            baddie_start_point_y;

    float
            xfloat,	//current position as floating point based on pixel pos - allows for infinite speed
            yfloat;	// variations because the xfloat can be reduced to xpix

    shared_ptr<SDL_Surface>
            ghostEl[5];

    BHeap
            heap;

    bool
            dirClear[3][3],	//dirClear[x][y], direction clear = 1, blocked = 0
            gateopen,		// determines if baddies can pass gate
            *closed;

    std::string
            filename;
};
