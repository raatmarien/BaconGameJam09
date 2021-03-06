/*
Copyright (C) 2015  Marien Raat

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <chunk.h>
#include <playerInventory.h>
#include <SFML/Graphics.hpp>
#include <vector>

struct WorldSettings {
    sf::Vector2i worldSize; // In chunks
    /* int skySize; // Ammount of tiles from top to be left unnocupied */
    sf::Texture *chunkTexture;
    PlayerInventory *inventory;
    ChunkSettings chunkSettings;
};

struct TileDamage {
    int totalTileX;
    int totalTileY;
    float maxStrength;
    float currentStrength;
    sf::RectangleShape *shape;
};

struct OrePlaceSettings {
    int minY;
    int maxY;
    int chanceN;
    int baseChanceT;
    int yChanceT;
    int triesPerY;
    int tilesTries;
    int tilesSpread;
    char tile;
}; 

class ChunkManager {
public:
    void initialize(WorldSettings settings);
    void hitTile(sf::Vector2i globalPixelPosition, float damage
                 , sf::Vector2f playerPosition, float hitRadius);
    void update(sf::View *currentView);
    void draw(sf::RenderTarget *target);
private:
    char **tiles; // Array of array of tiles to be copied by chunks
    Chunk **chunks; // Array of chunk pointers
    WorldSettings settings;

    int ammountOfChunks, tilesInChunk;
    int tilesInHeight, tilesInWidth;

    std::vector<TileDamage*> tileDamages;

    void setupWorld();
    void setLocalTile(int x, int y, char tile);
    void setupChunks();

    void addOre(OrePlaceSettings settings);

    void addDiamond();

    float getTileStrength(char tile);

    int getTileMoney(char tile);
};
