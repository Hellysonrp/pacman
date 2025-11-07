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
#include <fstream>
#include <iomanip>
#include <vector>

#define MAXENTRIES 8


class hScore
{
public:
    struct ScoreEntry {
        std::string playerName;
        unsigned long playerScore;
    };

    hScore();
    ~hScore();

    void add(std::string, unsigned int);

    std::string getName(int);

    int getScore(int);

    int save();
    int load();
    void clear();

    bool onlist(unsigned int) const;

    void setfilename(std::string fn);
    std::vector<ScoreEntry> getEntries() const;

private:
    void sortEntries();
    void compactEntries();
    void ensureSize();
    bool loadPlaintext(const std::string& data);
    bool loadLegacy(const std::string& data);
    std::string resolveExistingPath() const;
    std::string resolveWritablePath() const;

    std::vector<ScoreEntry> entries;
    std::string filename;
    std::string resolvedFilename;
};
