/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "BckgrObj.h"
#include <filesystem>

extern Log logtxt;
extern App app;
extern Game game;
extern Settings settings;



void BckgrObj::Draw(int ix, int iy, int obj, int type, int alp) {
    SDL_Rect pos;

    pos.x=ix;
    pos.y=iy;
    pos.h=pos.w=TILE_SIZE;

    if (type == 1) {
        drawSprite(objEl[obj], pos, alp);
    }
    else {
        drawSprite(mapEl[obj], pos, alp);
    }
}

void BckgrObj::Draw(int ix, int iy, int obj, int type) {
    Draw(ix,iy,obj,type,ALPHA_OPAQUE);
}

void BckgrObj::setFruitAlpha(int a) {
    fruitalpha = a;
}

void BckgrObj::drawSprite(shared_ptr<SDL_Texture> texture, SDL_Rect& position, int alpha) {
    SDL_SetTextureAlphaMod(texture.get(), alpha);
    SDL_RenderCopy(renderer, texture.get(), NULL, &position);
}

void BckgrObj::Draw() {
    int *map(NULL),
    *objmap(NULL),
    height,
    width,
    i,
    j;
    SDL_Rect pos;

    height = settings.fieldheight;
    width = settings.fieldwidth;
    game.getMaps(&map, &objmap);

    objcounter = 0;

    // Render background texture full screen
    SDL_Rect bgRect = {0, 0, settings.width, settings.height};
    SDL_RenderCopy(renderer, mapEl[0].get(), NULL, &bgRect);

    //DRAW FIELD
    for (j=0;j<height;j++) {
        for (i=0; i<width; i++) {

            pos.x=i*settings.tilesize;
            pos.y=j*settings.tilesize;
            pos.h=TILE_SIZE;
            pos.w=TILE_SIZE;

            if (map[j*width+i]==HORIZONTAL_WALL	&&	// horizontal line
                ( map[j*width+i+1] != EMPTY || i == width-1 ) &&
                ( map[j*width+i-1] != EMPTY || i == 0 ) ) {
                drawSprite(mapEl[1], pos, alpha);
            }

            else if (map[j*width+i]==HORIZONTAL_WALL)	{	// vertical line
                drawSprite(mapElRot[1][0], pos, alpha);
            }

            else if (map[j*width+i]==GHOST_DOOR &&		//ghost door
                     map[j*width+i + 1] != EMPTY &&
                     map[j*width+i - 1] != EMPTY) {
                drawSprite(mapEl[2], pos, alpha);
            }
            else if (map[j*width+i]==GHOST_DOOR)	{	// vertical ghost door
                drawSprite(mapElRot[2][0], pos, alpha);
            }

            else if (map[j*width+i]==UPPER_LEFT_CORNER) {		//upper left corner
                drawSprite(mapEl[3], pos, alpha);
            }
            else if (map[j*width+i]==UPPER_RIGHT_CORNER) {		// upper right corner
                drawSprite(mapEl[4], pos, alpha);
            }
            else if (map[j*width+i]==LOWER_RIGHT_CORNER) {		// lower  right corner
                drawSprite(mapEl[5], pos, alpha);
            }
            else if (map[j*width+i]==LOWER_LEFT_CORNER) {		// lower left corner
                drawSprite(mapEl[6], pos, alpha);
            }

            else if (map[j*width+i]==LEFT_T && 		// left T
                     ( map[j*width+i-1]==EMPTY || i == 0 ) ) {
                drawSprite(mapEl[7], pos, alpha);
            }
            else if (map[j*width+i]==LEFT_T)	{	// upside down T
                drawSprite(mapElRot[7][0], pos, alpha);
            }
            else if (map[j*width+i]==RIGHT_T &&		// right T
                     ( map[j*width+i+1]==EMPTY || i == width-1 ) ) {
                drawSprite(mapEl[8], pos, alpha);
            }
            else if (map[j*width+i]==RIGHT_T)	{	// upright T
                drawSprite(mapElRot[8][0], pos, alpha);
            }

            else if (map[j*width+i]==STUB &&
                     map[j*width+i-1] != EMPTY &&
                     map[j*width+i-1] != GHOST_DOOR &&
                     i > 0 )	 {//right stub
                drawSprite(mapEl[9], pos, alpha);
            }
            else if (map[j*width+i]==STUB &&
                     map[j*width+i+1] != EMPTY &&
                     map[j*width+i+1] != GHOST_DOOR &&
                     i < width-1) {	// left stub
                drawSprite(mapElRot[9][1], pos, alpha);
            }
            else if (map[j*width+i]==STUB &&
                     map[(j+1)*width+i] != EMPTY &&
                     map[(j+1)*width+i] != GHOST_DOOR &&
                     j < height -1) {	// upper stub
                drawSprite(mapElRot[9][0], pos, alpha);
            }
            else if (map[j*width+i]==STUB)	{// lower stub
                drawSprite(mapElRot[9][2], pos, alpha);
            }
        }
    }

    //DRAW OBJECTS

    for (j=0;j<height;j++) {
        for (i=0; i<width; i++) {

            pos.x=i*settings.tilesize+TILE_OFFSET; // +TILE_OFFSET are needed for correct placement
            pos.y=j*settings.tilesize+TILE_OFFSET;
            pos.h=TILE_SIZE;
            pos.w=TILE_SIZE;

            if (objmap[j*width+i]==SMALL_DOT) {
                drawSprite(objEl[1], pos, alpha);
                objcounter++;
            }
            if (objmap[j*width+i]==LARGE_DOT) {	// BIG DOTS!
                drawSprite(objEl[2], pos, alpha);
                objcounter++;
            }
            if (objmap[j*width+i]==FRUIT && specialspawned && !specialeaten) {	// fruit
                drawSprite(objEl[3], pos, fruitalpha);
                objcounter++;
            }
        }
    }


}

bool BckgrObj::LoadTextures(std::string path) {

    int i;
    std::string num[NUM_OF_MAP_TEXTURES];
    SDL_PixelFormat *fmt;
    auto texture_deleter = [](SDL_Texture* t) { SDL_DestroyTexture(t); };

    for (i=0;i<NUM_OF_MAP_TEXTURES;i++)
        num[i]='0'+i;

    try {
        std::filesystem::path basePath(path);
        for (i=0;i<NUM_OF_MAP_TEXTURES;i++) {
            shared_ptr<SDL_Surface> tempSurface(IMG_Load((basePath / ("m" + num[i] + ".png")).string().c_str()), SDL_FreeSurface);
            if ( tempSurface == NULL )
                throw Error(num[i] + "Failed to load map texture");

            //get pixel format from surface
            fmt=tempSurface->format;
            //set the transparent color key to RGB 255 0 255
            SDL_SetColorKey(tempSurface.get(), SDL_TRUE, SDL_MapRGB(fmt,255,0,255));

            // Convert surface to texture
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, tempSurface.get());
            if (tex == NULL)
                throw Error("Failed to create texture from surface: " + num[i]);
            mapEl[i].reset(tex, texture_deleter);

            // Create rotated versions
            for (int j=0;j<3;j++) {
                shared_ptr<SDL_Surface> rotatedSurface = Rotate(tempSurface, (j+1)*90);
                SDL_Texture* rotTex = SDL_CreateTextureFromSurface(renderer, rotatedSurface.get());
                if (rotTex == NULL)
                    throw Error("Failed to create rotated texture: " + num[i]);
                mapElRot[i][j].reset(rotTex, texture_deleter);
            }
        }
        for (i=1;i<5;i++) {
            shared_ptr<SDL_Surface> tempSurface(IMG_Load((basePath / ("o" + num[i] + ".png")).string().c_str()), SDL_FreeSurface);
            if ( tempSurface == NULL )
                throw Error(num[i] + "Failed to load object texture");

            //get pixel format from surface
            fmt=tempSurface->format;
            //set the transparent color key to RGB 255 0 255
            SDL_SetColorKey(tempSurface.get(), SDL_TRUE, SDL_MapRGB(fmt,255,0,255));

            // Convert surface to texture
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, tempSurface.get());
            if (tex == NULL)
                throw Error("Failed to create object texture from surface: " + num[i]);
            objEl[i].reset(tex, texture_deleter);
        }

        logtxt.print("Field textures loaded");
    }
    catch ( Error &err) {
        std::cerr << err.getDesc().c_str();
        app.setQuit(true);
        logtxt.print( err.getDesc() );
        return false;
    }
    catch ( ... ) {
        std::cerr << "Unexpected exception in BckgrObj::LoadTextures";
        app.setQuit(true);
        logtxt.print( "Unexpected error during Game()" );
        return false;
    }
    return true;
}

BckgrObj::BckgrObj( SDL_Renderer* renderer, int os)
    :	Object( renderer, os),
    objcounter(0),
    fruitalpha(ALPHA_OPAQUE),
    specialspawned(false),
    specialeaten(false)
{
}
