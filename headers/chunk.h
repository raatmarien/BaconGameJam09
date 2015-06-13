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

#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>

struct ChunkSettings {
    sf::Vector2i chunkSize;
    sf::Vector2i tileSize;
    sf::Vector2f position;
    sf::Vector2f tileTexSize;
    int tilesPerWidthTex; // Ammount of tiles in the width of the texture
    int scale;
    b2World *world;
};


class Chunk : public sf::Drawable, public sf::Transformable {
public:
    // Tiles is an array of chars of the size of chunkSize.x * chunkSize.y
    Chunk(ChunkSettings *settings, char *tiles);
    // Remove a tile from the chunk
    void removeTile(sf::Vector2i positionInChunk);
    // Add a tile to the world
    void addTile(sf::Vector2i positionInChunk, char tile);
    // Gets the tiletype from the tiles array
    char getTile(sf::Vector2i positionInChunk);
    // Adds the tiles to the b2World
    void moveInFocus();
    // Removes the tiles from the b2World
    void moveOutFocus();
    // IntRect for intersections
    sf::IntRect chunkRect;
private:
    bool inFocus;
    char *tiles;
    b2Fixture **fixtures;
    sf::Vector2i chunkSize, tileSize;
    sf::Vector2f tileTexSize;
    int scale;
    int ammountOfVisibleTiles, arraySize;
    int tilesPerWidthTex;
    sf::VertexArray tileQuads;
    b2Body *chunkBody;
    b2World *world;

    b2FixtureDef standardTileDef;
    b2PolygonShape standardTileShape;
    b2Vec2 standardTileSize; // Tile size in Box2D
    b2BodyDef bodyDef;

    void updateVertexArray();

    virtual void draw(sf::RenderTarget &target
                      , sf::RenderStates states) const;
};
