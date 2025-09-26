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

#define MAXENTRIES 8


class hScore
{
    // TODO: estruturar ScoreEntry para devolver nome e pontuação juntos
//     struct ScoreEntry {
//         std::string playerName;
//         unsigned long playerScore;
//     };
private:
    std::string
            name[MAXENTRIES];
    unsigned long
            score[MAXENTRIES];
    char
            filename[20];
    // TODO: método interno para ordenar vetores após operações de leitura/adição
public:
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
    // TODO: disponibilizar ranking completo
    //     std::vector<ScoreEntry> getRanking() const;
};
