/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "Ghost.h"

#define GHOSTSIZE 40

extern Log logtxt;
extern App app;
extern Settings settings;

void Ghost::Draw(int ix, int iy, int obj, int type) {
    SDL_Rect pos;

    pos.x=ix;
    pos.y=iy;
    pos.w=pos.h=GHOSTSIZE;

    SDL_SetAlpha(ghostEl[0].get(),SDL_SRCALPHA|SDL_RLEACCEL,alpha);
    SDL_BlitSurface(ghostEl[0].get(),NULL,buf.get(),&pos);
}
int Ghost::getXpix() {
    return pixelPos.getX();
}

int Ghost::getYpix() {
    return pixelPos.getY();
}
void Ghost::setTarget(int ix, int iy) {
    target.set(ix, iy);
}
bool Ghost::collision(int xtmp, int ytmp) {
    //error check
    if (xtmp < 0 || xtmp >= width || ytmp < 0 || ytmp >= height ) return 1;
    //collision detection
    if ( map[ ( ytmp ) * width + (xtmp) ] == 0 &&
         map[ ( ytmp ) * width + (xtmp + 1) ] == 0 &&
         map[ ( ytmp + 1 ) * width + (xtmp) ] == 0 &&
         map[ ( ytmp + 1 ) * width + (xtmp + 1) ] == 0 )
        return 0;
    return 1;
}
void Ghost::changeDifficulty(int spd, int iq) {
    defspeed += spd;
    baddie_iq += iq;
}
void Ghost::reset(int ix, int iy) {
    animcounter=0;
    tilePos.set(ix, iy);
    pixelPos = tilePos.toPixels(tilesize);
    xfloat= (float) pixelPos.getX();
    yfloat= (float) pixelPos.getY();
    direction.set(0, 0);
    nextDirection.set(0, 0);

    spdmod=defspeed;
    state=0;
    gateopen=true;
}
int Ghost::getState() {
    return state;
}
bool Ghost::find() {
    int
            i,
            currentx = pixelPos.getX() / tilesize,
            currenty = pixelPos.getY() / tilesize,
            tmppos;

    //if target is not clear

    if ( collision( target.getX(), target.getY()) ) return 0;

    // reset arrays

    for (i=0; i<width*height; i++) {
        closed[i]=0;
        parentDir[i]=4;
        closedF[i]=-1;
        Gstore[i]=-1;
    }

    // reset heap

    heap.reset();

    //init loop

    parentDir[ currenty*width + currentx ]= -1;
    Gstore[ tilePos.getY()*width + tilePos.getX() ] = 0;
    heap.add( currentx , currenty , calcF(currentx,currenty) );

    // loop start

    while ( true ) {

        //set focus coords

        currentx=heap.getX();
        currenty=heap.getY();

        // remember F, set status as closed

        closedF[currenty*width+currentx]=heap.getF();
        closed[currenty*width+currentx]=1;

        // if current tile == target, terminate loop

        if (currentx == target.getX() && currenty == target.getY()) return 1;

        //remove from open list

        heap.remove();

        // first up: RIGHT NEIGHBOR SQUARE
        // special case : current == width-2

        i=1;
        if ( currentx == width-2 ) {

            //if neighbor tile is clear and not on closed list

            if ( ! collision_ignore_gate(0, currenty) && !closed[currenty*width+0] ) {

                // if not already on openlist

                if ( !(tmppos=heap.find(0, currenty)) ) {

                    Gstore[(currenty)*width+0]=Gstore[currenty*width+currentx] + 1;

                    heap.add(0, currenty, calcF(0, currenty));	//add to openlist

                    parentDir[currenty*width+0]=3;				//set parent square
                }

                // if already on openlist -> this path lower G?

                else {
                    if ( Gstore[currenty*width+0] > Gstore[currenty*width+currentx] + 1 ) {

                        Gstore[currenty*width+0] = Gstore[currenty*width+currentx] + 1;	//update G
                        parentDir[currenty*width+0]=3;							//update parent direction
                        heap.changeF( heap.find(0, currenty), calcF(0,currenty));	//update F stored in openList
                    }
                }
            }
        }

        // usual case: current != width -2

        else {
            if (! collision_ignore_gate(currentx+i, currenty) && !closed[currenty*width+currentx +i] ) {
                if ( !(tmppos=heap.find(currentx+i, currenty)) ) {
                    Gstore[(currenty)*width+currentx + i]=Gstore[currenty*width+currentx] + 1;
                    heap.add(currentx+i, currenty, calcF(currentx+i, currenty));
                    parentDir[currenty*width+currentx+i]=3;
                }
                //if already on openlist
                else {			//if this path has a lower G
                    if ( Gstore[currenty*width+currentx+i] > Gstore[currenty*width+currentx] + 1 ) {
                        Gstore[currenty*width+currentx+i] = Gstore[currenty*width+currentx] + 1;
                        parentDir[currenty*width+currentx+i]=3;
                        heap.changeF( heap.find(currentx+i, currenty), calcF(currentx+i,currenty));
                    }
                }
            }
        }

        // LEFT NEIGHBOR SQUARE
        //special case : currentx == 0

        i = -1;

        if (currentx == 0 ) {
            if (! collision_ignore_gate(width-2, currenty) && !closed[currenty*width+width-2] ) {
                if ( !(tmppos=heap.find(width-2, currenty)) ) {
                    Gstore[(currenty)*width+width-2]=Gstore[currenty*width+currentx] + 1;
                    heap.add(width-2, currenty, calcF(width-2, currenty));
                    parentDir[currenty*width+width-2]=1;
                }
                else {
                    if ( Gstore[currenty*width+width-2] > Gstore[currenty*width+currentx] + 1 ) {
                        Gstore[currenty*width+width-2] = Gstore[currenty*width+currentx] + 1;
                        parentDir[currenty*width+width-2]=1;
                        heap.changeF( heap.find(width-2, currenty), calcF(width-2,currenty));
                    }
                }
            }
        }

        // normal case
        else {
            if (! collision_ignore_gate(currentx+i, currenty) && !closed[currenty*width+currentx+i] ) {
                if ( !(tmppos=heap.find(currentx+i, currenty)) ) {
                    Gstore[(currenty)*width+currentx + i]=Gstore[currenty*width+currentx] + 1;
                    heap.add(currentx+i, currenty, calcF(currentx+i, currenty));
                    parentDir[currenty*width+currentx+i]=1;
                }
                else {
                    if ( Gstore[currenty*width+currentx+i] > Gstore[currenty*width+currentx] + 1 ) {
                        Gstore[currenty*width+currentx+i] = Gstore[currenty*width+currentx] + 1;
                        parentDir[currenty*width+currentx+i]=1;
                        heap.changeF( heap.find(currentx+i, currenty), calcF(currentx+i,currenty));
                    }
                }
            }
        }

        // LOWER NEIGHBOR SQUARE

        i=1;

        // special case

        if ( currenty == height -2 ) {
            if ( ! collision_ignore_gate(currentx, 0) && !closed[0*width+currentx] ) {

                // if not already on openlist

                if ( !(tmppos=heap.find(currentx, 0)) ) {

                    Gstore[(0)*width+currentx]=Gstore[currenty*width+currentx] + 1;

                    heap.add(currentx, 0, calcF(currentx, 0));	//add to openlist

                    parentDir[0*width+currentx]=0;				//set parent square
                }

                // if already on openlist -> this path lower G?

                else {
                    if ( Gstore[0*width+currentx] > Gstore[currenty*width+currentx] + 1 ) {

                        Gstore[0*width+currentx] = Gstore[currenty*width+currentx] + 1;	//update G
                        parentDir[0*width+currentx]=0;							//update parent direction
                        heap.changeF( heap.find(currentx, 0), calcF(currentx, 0));	//update F stored in openList
                    }
                }
            }
        }

        //normal case

        else {

            if (! collision_ignore_gate(currentx, currenty+i) && !closed[(currenty+i)*width+currentx] ) {
                if ( !(tmppos=heap.find(currentx, currenty+i)) ) {
                    Gstore[(currenty+i)*width+currentx]=Gstore[currenty*width+currentx] + 1;
                    heap.add(currentx, currenty+i, calcF(currentx, currenty+i));
                    parentDir[(currenty+i)*width+currentx]=0;
                }
                else {
                    if ( Gstore[(currenty+i)*width+currentx] > Gstore[currenty*width+currentx] + 1 ) {
                        Gstore[(currenty+i)*width+currentx] = Gstore[currenty*width+currentx] + 1;
                        parentDir[(currenty+i)*width+currentx]=0;
                        heap.changeF( heap.find(currentx, currenty+i), calcF(currentx,currenty+i));
                    }
                }
            }
        }

        // UPPER NEIGHBOR SQUARE

        i=-1;

        //special case
        if (currenty == 0 ) {
            if (! collision_ignore_gate(currentx, height-2) && !closed[(height-2)*width+currentx] ) {
                if ( !(tmppos=heap.find(currentx, height-2)) ) {
                    Gstore[(height-2)*width+currentx]=Gstore[currenty*width+currentx] + 1;
                    heap.add(currentx, height-2, calcF(currentx, height-2));
                    parentDir[(height-2)*width+currentx]=2;
                }
                else {
                    if ( Gstore[(height-2)*width+currentx] > Gstore[currenty*width+currentx] + 1 ) {
                        Gstore[(height-2)*width+currentx] = Gstore[currenty*width+currentx] + 1;
                        parentDir[(height-2)*width+currentx]=2;
                        heap.changeF( heap.find(currentx, height-2), calcF(currentx,height-2));
                    }
                }
            }
        }

        //normal case
        else {
            if (! collision_ignore_gate(currentx, currenty+i) && !closed[(currenty+i)*width+currentx] ) {
                if ( !(tmppos=heap.find(currentx, currenty+i)) ) {
                        Gstore[(currenty+i)*width+currentx]=Gstore[currenty*width+currentx] + 1;
                        heap.add(currentx, currenty+i, calcF(currentx, currenty+i));
                        parentDir[(currenty+i)*width+currentx]=2;
                }
                else {
                    if ( Gstore[(currenty+i)*width+currentx] > Gstore[currenty*width+currentx] + 1 ) {
                        Gstore[(currenty+i)*width+currentx] = Gstore[currenty*width+currentx] + 1;
                        parentDir[(currenty+i)*width+currentx]=2;
                        heap.changeF( heap.find(currentx, currenty+i), calcF(currentx,currenty+i));
                    }
                }
            }
        }

        // if open list is empty, terminate

        if ( heap.isEmpty() ) return 0;
    }
}
int Ghost::calcF(int ix, int iy) const {
    int a,b;

    //distance current tile -> start tile

    a= Gstore[(iy)*width+ix] ;

    // x-distance current tile -> target tile

    b= ix > target.getX() ? (ix-target.getX()) : (target.getX()-ix);  //current to target =H

    //special cases: target left edge, baddie right edge

    if ( ( width-1)-ix+target.getX() < b ) 	b=(width-2)-ix+target.getX();	//width -1 to get correct F

    //				target right edge, baddie left edge

    if (ix + (width+1) -target.getX() < b )	b=ix + (width-2) -target.getX();

    a+=b;

    // y-distance current tile -> target tile

    b= iy > target.getY() ? (iy-target.getY()) : (target.getY()-iy);

    //special case: target upper edge, baddie lower edge

    if ( ( height-1)-iy+target.getY() < b ) 	b=(height-2)-iy+target.getY();

    // vice versa

    if (iy + (height+1) -target.getY() < b )	b=iy + (height-2) -target.getY();

    a+=b;

    return a;
}
bool Ghost::tracePath() {
    int xtmp, ytmp;

    //reset dirToTar

    dirToTar = -1;

    //set coords

    xtmp= heap.getX();
    ytmp= heap.getY();

    if (ytmp == target.getY() && xtmp == target.getX() ) {	//error check

        while ( parentDir[ ytmp*width + xtmp ] != -1 ) {

            dirToTar = parentDir[ytmp*width+xtmp];		//not sure about dtotarget -1, maybe without -1

            if (dirToTar == 0) {
                if (ytmp==0) ytmp=height-2;
                else ytmp--;
            }
            else if (dirToTar == 1) {
                if ( xtmp== width-2) xtmp=0;
                else xtmp++;
            }
            else if (dirToTar == 2) {
                if (ytmp == height -2 ) ytmp=0;
                else ytmp++;
            }
            else if (dirToTar == 3) {
                if (xtmp == 0) xtmp= width-2;
                else xtmp--;
            }
        }

        if ( dirToTar == 0 ) dirToTar = 2;
        else if ( dirToTar == 1 ) dirToTar = 3;
        else if ( dirToTar == 2 ) dirToTar = 0;
        else if ( dirToTar == 3 ) dirToTar = 1;

        if (dirToTar < 0 || dirToTar > 3 ) return 1;	//error check

        return 0;
    }

    else return 1;	//error
}
void Ghost::pathCalcVuln() {
    bool flag = 0;
    int newdir = -1;


    //find path
    if ( find() ) flag=tracePath();

    // RANDOM PATH, one not in shortest direction to pacman is preferred (dirToTar)

    //if within the starting square
    if (gateopen) {
        if ( dirToTar == 0) {
            nextDirection.set(0, -1);
        }
        else if ( dirToTar == 1 ) {
            nextDirection.set(1, 0);
        }
        else if ( dirToTar == 2 ) {
            nextDirection.set(0, 1);
        }
        else if ( dirToTar == 3 ) {
            nextDirection.set(-1, 0);
        }
    }

    //if dead end
    else if ( !dirClear[1+direction.getX()][1+direction.getY()] && !dirClear[1+direction.getY()][1+direction.getX()] && !dirClear[1-direction.getY()][1-direction.getX()] && direction.getX() != direction.getY() ) {
        nextDirection.set(-direction.getX(), -direction.getY());
    }
    //generate random dir != - current dir
    else {
        nextDirection.set(-direction.getX(), -direction.getY());
        while ( nextDirection.getX() == -direction.getX() && nextDirection.getY() == -direction.getY() ) {

            newdir = rand()%4;

            //the following 2 lines make the baddies prefer directions other than shortest way to pacman

            if (newdir == dirToTar ) newdir = rand()%4;
            if (newdir == dirToTar ) newdir = rand()%4;

            if ( newdir == 0 ) {
                nextDirection.set(0, -1);
            }
            else if ( newdir == 1 ) {
                nextDirection.set(1, 0);
            }
            else if ( newdir == 2 ) {
                nextDirection.set(0, 1);
            }
            else if ( newdir == 3 ) {
                nextDirection.set(-1, 0);
            }
            if ( !dirClear[1+nextDirection.getX()][1+nextDirection.getY()] ) {
                nextDirection.set(-direction.getX(), -direction.getY());
            }
        }
    }

    direction=nextDirection;
}

void Ghost::pathCalcDead() {
    bool
            flag = 0;
    int
            newdir = -1,
            cur_opp_dir=-1;	// opposite of current direction

    //translate dx + dy into dir -> 0 = up, 1 = right, 2 = down, 3 = left

    if (direction.getX() == 1) cur_opp_dir = 3;
    else if (direction.getX() == -1) cur_opp_dir = 1;
    else if (direction.getY() == 1) cur_opp_dir = 0;
    else if (direction.getY() == -1) cur_opp_dir = 2;



    target.set(baddie_start_point_x, baddie_start_point_y);

    //find path
    if ( find() ) flag=tracePath();

    //if find and trace successful
    // TRACE PATH

    if (! flag ) {		// pathfinding + trace successful
        if ( dirToTar == 0) {
            nextDirection.set(0, -1);
        }
        else if ( dirToTar == 1 ) {
            nextDirection.set(1, 0);
        }
        else if ( dirToTar == 2 ) {
            nextDirection.set(0, 1);
        }
        else if ( dirToTar == 3 ) {
            nextDirection.set(-1, 0);
        }
    }

    // ELSE RANDOM PATH	-- only happens of trace not successful
    else {

        //if dead end
        if ( !dirClear[1+direction.getX()][1+direction.getY()] && !dirClear[1+direction.getY()][1+direction.getX()] && !dirClear[1-direction.getY()][1-direction.getX()] && direction.getX() != direction.getY() ) {
            nextDirection.set(-direction.getX(), -direction.getY());
        }
        //generate random dir != - current dir
        else {
            nextDirection.set(-direction.getX(), -direction.getY());
            while ( nextDirection.getX() == -direction.getX() && nextDirection.getY() == -direction.getY() ) {

                newdir = rand()%4;

                if ( newdir == 0 ) {
                    nextDirection.set(0, -1);
                }
                else if ( newdir == 1 ) {
                    nextDirection.set(1, 0);
                }
                else if ( newdir == 2 ) {
                    nextDirection.set(0, 1);
                }
                else if ( newdir == 3 ) {
                    nextDirection.set(-1, 0);
                }
                if ( !dirClear[1+nextDirection.getX()][1+nextDirection.getY()] ) {
                    nextDirection.set(-direction.getX(), -direction.getY());
                }
            }
        }
    }
    direction=nextDirection;
}

void Ghost::pathCalcNormal() {
    bool
            flag = 0;
    int
            newdir = -1,
            cur_opp_dir=-1;	// opposite of current direction

    //translate dx + dy into dir -> 0 = up, 1 = right, 2 = down, 3 = left

    if (direction.getX() == 1) cur_opp_dir = 3;
    else if (direction.getX() == -1) cur_opp_dir = 1;
    else if (direction.getY() == 1) cur_opp_dir = 0;
    else if (direction.getY() == -1) cur_opp_dir = 2;


    if ( gateopen && !collision(settings.gatex, settings.gatey) )
        setTarget(settings.gatex, settings.gatey);

    //find path
    if ( find() ) flag=tracePath();

    //if find and trace successful, random roll successful AND calculated direction != opposite of current dir
    // chance based on Gstore[target square] = distance
    // TRACE PATH

    if (! flag &&		// pathfinding + trace successful
        rand()%(( width+height) / 2 ) + baddie_iq > Gstore[ target.getY() * width + target.getX() ]  && // random roll successful
        dirToTar != cur_opp_dir) {	//and pathfinding direction is not the opposite of current direction

        if ( dirToTar == 0) {
            nextDirection.set(0, -1);
        }
        else if ( dirToTar == 1 ) {
            nextDirection.set(1, 0);
        }
        else if ( dirToTar == 2 ) {
            nextDirection.set(0, 1);
        }
        else if ( dirToTar == 3 ) {
            nextDirection.set(-1, 0);
        }
    }

    // ELSE RANDOM PATH
    else {

        //if dead end
        if ( !dirClear[1+direction.getX()][1+direction.getY()] && !dirClear[1+direction.getY()][1+direction.getX()] && !dirClear[1-direction.getY()][1-direction.getX()] && direction.getX() != direction.getY() ) {
            nextDirection.set(-direction.getX(), -direction.getY());
        }
        //generate random dir != - current dir
        else {
            nextDirection.set(-direction.getX(), -direction.getY());
            while ( nextDirection.getX() == -direction.getX() && nextDirection.getY() == -direction.getY() ) {

                newdir = rand()%4;

                if ( newdir == 0 ) {
                    nextDirection.set(0, -1);
                }
                else if ( newdir == 1 ) {
                    nextDirection.set(1, 0);
                }
                else if ( newdir == 2 ) {
                    nextDirection.set(0, 1);
                }
                else if ( newdir == 3 ) {
                    nextDirection.set(-1, 0);
                }
                if ( !dirClear[1+nextDirection.getX()][1+nextDirection.getY()] ) {
                    nextDirection.set(-direction.getX(), -direction.getY());
                }
            }
        }
    }
    direction=nextDirection;
}

bool Ghost::collision_ignore_gate(int xtmp, int ytmp) {
    //error check
    if (xtmp < 0 || xtmp >= width || ytmp < 0 || ytmp >= height ) return 1;
    //collision detection
    if ( ( map[ ( ytmp ) * width + (xtmp) ] == 0 || map[ ( ytmp ) * width + (xtmp) ] == 2 ) &&
         ( map[ ( ytmp ) * width + (xtmp + 1) ] == 0 || map[ ( ytmp ) * width + (xtmp + 1) ] == 2 ) &&
         ( map[ ( ytmp +1 ) * width + (xtmp) ] == 0  || map[ ( ytmp +1 ) * width + (xtmp) ] == 2 )&&
         ( map[ ( ytmp + 1 ) * width + (xtmp + 1) ] == 0 || map[ ( ytmp + 1 ) * width + (xtmp + 1) ] == 2 ) )
        return 0;
    return 1;
}
void Ghost::setState(int st) {
    int curdir;

    //vulnerable mode
    if (st == 1 && state != 3) {
        state = st;

        spdmod= 2*defspeed/3;

        if (direction.getX() == 1) curdir=1;
        else if (direction.getX() == -1) curdir=3;
        else if (direction.getY() == 1) curdir=2;
        else if (direction.getY() == -1) curdir=0;

        if (curdir == dirToTar ) {
            direction.set(-direction.getX(), -direction.getY());
        }
    }
    //warning mode
    else if (st == 2 && state == 1) {
        state = st;
        spdmod= 2*defspeed/3;
        animcounter=0;
    }
    //dead mode
    else if (st == 3 && state != 0) {
        gateopen = 1;
        state = st;
        spdmod= 4*defspeed/3;
    }
    //normal mode
    else if (st == 0 && state == 3) {
        gateopen = 1;
        state = st;
        spdmod =defspeed;
    }
    else if (st == 0 && state != 0) {
        state = st;
        spdmod =defspeed;
    }
}
void Ghost::Update( int time) {
    bool
            cont = 0;
    int
            oldx,
            oldy;

    //if target reached, set normal state

    if ( state == 3 &&
        tilePos.getX() == baddie_start_point_x &&
        tilePos.getY() == baddie_start_point_y) setState(0);


    //screen wrappers

    if ( tilePos.getX() == width-2 && direction.getX() == 1 ) {
        tilePos.setX(0);
        pixelPos.setX(0);
        xfloat = 0;
    }
    else if ( pixelPos.getX() == 0 && direction.getX() == -1 ) {
        tilePos.setX(width-2);
        pixelPos.setX(tilePos.getX()*tilesize);
        xfloat = (float)pixelPos.getX();
    }
    if ( tilePos.getY() == height-2 && direction.getY() == 1 ) {
        tilePos.setY(0);
        pixelPos.setY(0);
        yfloat = 0;
    }
    else if ( pixelPos.getY() == 0 && direction.getY() == -1 ) {
        tilePos.setY(height-2);
        pixelPos.setY(tilePos.getY()*tilesize);
        yfloat = (float)pixelPos.getY();
    }


    //remember old coords for adjustments at end

    oldx=pixelPos.getX();
    oldy=pixelPos.getY();

    // if at tile beginning
    // and we haven't processed this location yet
    if ( pixelPos.getX() % tilesize == 0 && pixelPos.getY() % tilesize == 0 &&
         !(pixelPos.getX() == pixelPosAtLastDirChange.getX() && pixelPos.getY() == pixelPosAtLastDirChange.getY()) ) {

        //init dirClear array
        //to the right
        if ( gateopen ) {
            //right
            if (collision_ignore_gate( pixelPos.getX() / tilesize + 1, pixelPos.getY() / tilesize ) ) dirClear[1+1][1+0]=0;
            else dirClear[1+1][1+0]=1;

            //left

            if (collision_ignore_gate( pixelPos.getX() / tilesize - 1, pixelPos.getY() / tilesize ) ) dirClear[1-1][1+0]=0;
            else dirClear[1-1][1+0]=1;

            //up

            if (collision_ignore_gate( pixelPos.getX() / tilesize , pixelPos.getY() / tilesize - 1) ) dirClear[1+0][1-1]=0;
            else dirClear[1+0][1-1]=1;

            //down

            if (collision_ignore_gate( pixelPos.getX() / tilesize , pixelPos.getY() / tilesize +1) ) dirClear[1+0][1+1]=0;
            else dirClear[1+0][1+1]=1;
        }
        else {
            //right
            if (collision( pixelPos.getX() / tilesize + 1, pixelPos.getY() / tilesize ) ) dirClear[1+1][1+0]=0;
            else dirClear[1+1][1+0]=1;

            //left

            if (collision( pixelPos.getX() / tilesize - 1, pixelPos.getY() / tilesize ) ) dirClear[1-1][1+0]=0;
            else dirClear[1-1][1+0]=1;

            //up

            if (collision( pixelPos.getX() / tilesize , pixelPos.getY() / tilesize - 1) ) dirClear[1+0][1-1]=0;
            else dirClear[1+0][1-1]=1;

            //down

            if (collision( pixelPos.getX() / tilesize , pixelPos.getY() / tilesize +1) ) dirClear[1+0][1+1]=0;
            else dirClear[1+0][1+1]=1;
        }

        // switch gateopen status if current tile is a gate tile.

        if ( map[ tilePos.getY()*width + tilePos.getX()] == 2 ) gateopen = !gateopen;

        // if a direction perpendicular to current direction is clear
        // OR current direction is blocked ( = dead end)
        // OR if dx == dy (starting state) set cont flag to 1.
        // cont flag determines whether new dir will be calculated or not

        if ( dirClear[1+direction.getY()][1+direction.getX()] || dirClear[1-direction.getY()][1-direction.getX()] || !dirClear[1+direction.getX()][1+direction.getY()] || direction.getX() == direction.getY() ) cont = 1;

        // if cont == 1, calc new direction
        // newdir cannot be the opposite of curdir UNLESS it is the only way.

        if (cont == 1 && state == 0) pathCalcNormal();
        else if (cont == 1 && ( state == 1 || state == 2) ) pathCalcVuln();
        else if (cont == 1 && state == 3 ) pathCalcDead();

        //store location
        pixelPosAtLastDirChange.setX(pixelPos.getX());
        pixelPosAtLastDirChange.setY(pixelPos.getY());
    }

    //MOVEMENT PART STARTS HERE


    //  dir    *       speed in percent
    xfloat += ( (float)(time * direction.getX() * spdmod) / MOVEMOD );
    yfloat += ( (float)(time * direction.getY() * spdmod) / MOVEMOD );

    //COORD ADJUSTMENTS

    if ( xfloat > (float)(tilePos.getX()+1)*tilesize ) xfloat = (float)(tilePos.getX()+1)*tilesize;
    if ( yfloat > (float)(tilePos.getY()+1)*tilesize ) yfloat = (float)(tilePos.getY()+1)*tilesize;

    if ( xfloat < (float)(tilePos.getX())*tilesize && oldx > (float)(tilePos.getX())*tilesize )
        xfloat = (float)(tilePos.getX())*tilesize;
    if ( yfloat < (float)(tilePos.getY())*tilesize && oldy > (float)(tilePos.getY())*tilesize)
        yfloat = (float)(tilePos.getY())*tilesize;

    //COORD UPDATES

    pixelPos.setX((int)xfloat);
    pixelPos.setY((int)yfloat);

    tilePos.setX(pixelPos.getX()/tilesize);
    tilePos.setY(pixelPos.getY()/tilesize);
}
void Ghost::Draw() {

    SDL_Rect pos;

    pos.x=pixelPos.getX();
    pos.y=pixelPos.getY();
    pos.h=GHOSTSIZE;
    pos.w=GHOSTSIZE;

    //normal state

    if (state == 0) {
        SDL_SetAlpha(ghostEl[0].get(),SDL_SRCALPHA|SDL_RLEACCEL,alpha);
        SDL_BlitSurface(ghostEl[0].get(),NULL,buf.get(),&pos);
    }

    //vulnerable state

    else if (state == 1) {
        SDL_SetAlpha(ghostEl[2].get(),SDL_SRCALPHA|SDL_RLEACCEL,alpha);
        SDL_BlitSurface(ghostEl[2].get(),NULL,buf.get(),&pos);
    }

    //warning state

    else if (state == 2) {
        if ( !paused ) animcounter++;
        if (animcounter%30 < 15) {
            SDL_SetAlpha(ghostEl[3].get(),SDL_SRCALPHA|SDL_RLEACCEL,alpha);
            SDL_BlitSurface(ghostEl[3].get(),NULL,buf.get(),&pos);
        }
        else {
            SDL_SetAlpha(ghostEl[2].get(),SDL_SRCALPHA|SDL_RLEACCEL,alpha);
            SDL_BlitSurface(ghostEl[2].get(),NULL,buf.get(),&pos);
        }
    }
    //if dead, only eyes are drawn

    else if (state == 3) {
        SDL_SetAlpha(ghostEl[4].get(),SDL_SRCALPHA|SDL_RLEACCEL,alpha);
        SDL_BlitSurface(ghostEl[4].get(),NULL,buf.get(),&pos);
    }

    if (direction.getX() == 1)
        pos.x=pos.x+2;
    else if (direction.getX() == -1)
        pos.x=pos.x-2;
    else if (direction.getY() == -1)
        pos.y=pos.y-2;
    else if (direction.getY() == 1)
        pos.y=pos.y+2;

    //draw eyes
    SDL_SetAlpha(ghostEl[1].get(),SDL_SRCALPHA|SDL_RLEACCEL,alpha);
    SDL_BlitSurface(ghostEl[1].get(),NULL,buf.get(),&pos);

}

bool Ghost::LoadTextures(std::string path) {

    std::string files[5];
    SDL_PixelFormat *fmt;

    files[0]=path + "baddie" + filename + ".png";
    files[1]=path + "baddie_eyes.png";
    files[2]=path + "baddie" + filename + "vuln.png";
    files[3]=path + "baddie" + filename + "warn.png";
    files[4]=path + "baddie_dead.png";

    try {

        for (int i = 0; i<5; i++) {
            ghostEl[i].reset(IMG_Load(files[i].c_str()), SDL_FreeSurface);
            if ( !ghostEl[i] )
                throw Error("Failed to load ghost texture: " + files[i]);

            fmt=ghostEl[i]->format;
            SDL_SetColorKey(ghostEl[i].get(),SDL_SRCCOLORKEY|SDL_RLEACCEL,SDL_MapRGB(fmt,255,0,255));
        }
        logtxt.print(filename + " ghost textures loaded");
    }
    catch ( Error &err) {
        std::cerr << err.getDesc();
        app.setQuit(true);
        logtxt.print( err.getDesc() );
        return false;
    }
    catch ( ... ) {
        std::cerr << "Unexpected exception in Ghost::LoadTextures()";
        app.setQuit(true);
        logtxt.print( "Unexpected error" );
        return false;
    }
    return true;
}

Ghost::Ghost(shared_ptr<SDL_Surface> buf, int os, int ix, int iy, int ispdmod, int itilesize,
			 int iheight, int iwidth, int *imap, std::string fn)
:   Object( buf, os),
    tilePos(ix, iy),
    pixelPos(ix * itilesize, iy * itilesize),
    direction(0, 0),
    nextDirection(0, 0),
    target(0, 0),
    pixelPosAtLastDirChange(1, 1),
    spdmod(ispdmod),
    tilesize(itilesize),
    height(iheight),
    width(iwidth),
    map(imap),
    dirToTar(-1),
    state(0),
    animcounter(0),
    baddie_iq(0),
    heap(iwidth, iheight),
    gateopen(1)
{
    int i,j;

    filename = fn;

    xfloat=(float)pixelPos.getX();
    yfloat=(float)pixelPos.getY();

    defspeed=spdmod;

    baddie_start_point_x=ix;
    baddie_start_point_y=iy;

    closed = new bool[height*width];
    parentDir = new int[height*width];
    Gstore = new int[height*width];
    closedF = new int[height*width];

    for (i=0;i<3;i++) for (j=0; j<3; j++) dirClear[j][i]=0;
}

Ghost::~Ghost()
{
    int i;

    if  ( closed != NULL ) delete[] closed;
    if ( parentDir != NULL ) delete[] parentDir;
    if ( Gstore != NULL ) delete[] Gstore;
    if ( closedF != NULL ) delete[] closedF;
}
