/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#pragma once
#include <vector>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <sys/stat.h>
#include <fstream>
#include <iostream>

#include "Constants.h"
#include "Log.h"
#include "Error.h"

using std::string;

class Settings {
public:
    Settings();
    ~Settings();

    bool LoadSettings(string filename);

    //searches for str in level/skinspaths; if successful, sets currently selected path.
    //throws Error when the path cannot be found
    void setPath(int mode, string str);

    //////////////////////////////
    // VARIABLES	- APP
    //////////////////////////////
    int
            width,
            height;

    //////////////////////////////
    // VARIABLES	- GAME
    //////////////////////////////

    int
            fieldwidth,
            fieldheight,
            tilesize,
            gatex,
            gatey,
            pacstartx,
            pacstarty,
            pacspeed,
            baddiestartx,
            baddiestarty,
            baddiespeed,
            baddieiq,
            vuln_duration,

            lvlpathcurrent,
            skinspathcurrent;

    std::vector<std::filesystem::path>
            lvlpath,
            skinspath,
            searchpaths;

    /* look for file in search paths and return first instance */
    string getFile(string filename);
};
