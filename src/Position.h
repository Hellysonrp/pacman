/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#pragma once

class Position {
private:
    int x, y;
    
public:
    Position(int x = 0, int y = 0) : x(x), y(y) {}
    
    // Getters
    int getX() const { return x; }
    int getY() const { return y; }
    
    // Setters
    void setX(int newX) { x = newX; }
    void setY(int newY) { y = newY; }
    void set(int newX, int newY) { x = newX; y = newY; }
    
    // Conversion methods
    Position toPixels(int tileSize) const {
        return Position(x * tileSize, y * tileSize);
    }
    
    Position toTiles(int tileSize) const {
        return Position(x / tileSize, y / tileSize);
    }
    
    // Utility methods
    int distanceTo(const Position& other) const {
        int dx = x - other.x;
        int dy = y - other.y;
        return (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy); // Manhattan distance
    }
    
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
    
    bool operator!=(const Position& other) const {
        return !(*this == other);
    }
    
    // Arithmetic operations for position calculations
    Position operator+(const Position& other) const {
        return Position(x + other.x, y + other.y);
    }
    
    Position operator-(const Position& other) const {
        return Position(x - other.x, y - other.y);
    }
};
