/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "Settings.h"

extern Log logtxt;

void Settings::setPath(int mode,std::string str) {
    switch (mode) {
    case MODE_LEVELS:
        for (size_t i = 0; i < lvlpath.size(); ++i) {
            if (lvlpath[i]=="./levels/" + str + "/") {
                lvlpathcurrent=static_cast<int>(i);
                return;
            }
        }
        throw Error("Level path not found: " + str);
    case MODE_SKINS:
        for (size_t i = 0; i < skinspath.size(); ++i) {
            if (skinspath[i]=="./skins/" + str + "/") {
                skinspathcurrent=static_cast<int>(i);
                return;
            }
        }
        throw Error("Skin path not found: " + str);
    default:
        throw Error("Unknown mode provided to Settings::setPath");
    }
}

string Settings::getFile(string filename) {
    struct stat fileInfo;
    for(int i = 0; i < searchpaths.size(); i++) {
        string path = searchpaths[i] + "/" + filename;
        if (stat(path.c_str(), &fileInfo) == 0) {
            return path;
        }
    }
    throw new Error("File not found: " + filename);
}

bool Settings::LoadSettings(std::string filename) {

    filename = getFile(filename);

    std::ifstream	file( filename.c_str() );
    std::string		buffer,
    tmpstr;
    char			c=';' ;
    int				pos;

    if (!file) {
        logtxt.print( "Could not open settings file" );
        return false;
    }

    while (file)	{
        do {
            if ( file.eof() ) break;
            c=file.get();
        } while (c < 'A' || c > 'z');

        pos= file.tellg();
        pos--;

        if (pos<0) pos=0;

        file.seekg(pos);

        getline(file, buffer, '=');
        if (! file.eof() ) {
            if (buffer == "WIDTH") file >> width;
            else if (buffer == "HEIGHT") file >> height;
            else if (buffer == "FIELDWIDTH") file >> fieldwidth;
            else if (buffer == "FIELDHEIGHT") file >> fieldheight;
            else if (buffer == "TILESIZE") file >> tilesize;
            else if (buffer == "PACSTARTX") file >> pacstartx;
            else if (buffer == "PACSTARTY") file >> pacstarty;
            else if (buffer == "PACSPEED") file >> pacspeed;
            else if (buffer == "BADDIESTARTX") file >> baddiestartx;
            else if (buffer == "BADDIESTARTY") file >> baddiestarty;
            else if (buffer == "BADDIESPEED") file >> baddiespeed;
            else if (buffer == "BADDIEIQ") file >> baddieiq;
            else if (buffer == "VULN_DURATION") file >> vuln_duration;
            else if (buffer == "GATEX") file >> gatex;
            else if (buffer == "GATEY") file >> gatey;
            else if (buffer == "LEVEL_PATH") {
                getline(file, tmpstr, ';');
                lvlpath.push_back("./levels/" + tmpstr + "/");
            }
            else if (buffer == "SKINS_PATH") {
                getline(file, tmpstr, ';');
                skinspath.push_back("./skins/" + tmpstr + "/");
            }
        }
    }

    file.close();

    logtxt.print(filename + " loaded");

    return true;
}

Settings::Settings() {
    width = 640;
    height = 480;

    lvlpathcurrent = 0;
    skinspathcurrent = 0;

    gatex = 0;
    gatey = 0;
    fieldwidth = 0;
    fieldheight = 0;
    tilesize = 0;
    pacstartx = 0;
    pacstarty = 0;
    pacspeed = 0;
    baddiestartx = 0;
    baddiestarty = 0;
    baddiespeed = 0;
    baddieiq = 0;
    vuln_duration = 0;

    searchpaths.push_back(".");
    searchpaths.push_back(string(getenv("HOME")) + "/" HOME_CONF_PATH);
    searchpaths.push_back(APP_PATH);
}

Settings::~Settings() {}
