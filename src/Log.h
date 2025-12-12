/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#pragma once
#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <cstdlib>

#include "Constants.h"
#include "App.h"
#include "Error.h"

class Log
{
public:
    Log();
    ~Log();

    bool setFilename(std::filesystem::path fn);

    bool print(std::string txt);
private:
    std::filesystem::path
            filename;
};
