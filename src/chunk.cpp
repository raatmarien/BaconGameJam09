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

#include <chunk.h>
#include <iostream>

using namespace sf;

Chunk::Chunk(ChunkSettings *settings, char *tiles) {
    this->chunkSize = settings->chunkSize;
    this->tileSize = settings->tileSize;
    this->scale = settings->scale;
    this->world = settings->world;
    this->tilesPerWidthTex = settings->tilesPerWidthTex;
    this->tileTexSize = settings->tileTexSize;

    setPosition(settings->position);

    chunkRect = IntRect(getPosition().x
                        , getPosition().y
                        , chunkSize.x * tileSize.x
                        , chunkSize.y * tileSize.y);

    arraySize = chunkSize.x * chunkSize.y;
    this->tiles = tiles;
    fixtures = new b2Fixture*[arraySize];

    bodyDef.type = b2_staticBody;
    bodyDef.position.Set(settings->position.x / scale
                         , settings->position.y / scale);

    standardTileDef.density = 1.0f;
    standardTileDef.friction = 1.0f;

    standardTileSize = b2Vec2((float) (tileSize.x) / (float) (scale)
                              , (float) (tileSize.y) / (float) (scale));

    ammountOfVisibleTiles = 0;

    for (int i = 0; i < arraySize; i++) {
        // *(this->tiles + i) = *(tiles + i);
        if (*(this->tiles + i)) { // tile is visible 
            ammountOfVisibleTiles++;
        }
    }
    inFocus = false;

    updateVertexArray();
}

void Chunk::removeTile(Vector2i positionInChunk) {
    int i = positionInChunk.x + positionInChunk.y * chunkSize.x;
    if (*(tiles + i) && i >= 0 && i < arraySize) {
        *(tiles + i) = 0;
        
        chunkBody->DestroyFixture(*(fixtures + i));
        
        ammountOfVisibleTiles--;

        updateVertexArray();
    }
}

void Chunk::addTile(Vector2i positionInChunk, char tile) {
    if (tile) {
        int i = positionInChunk.x + positionInChunk.y * chunkSize.x;
        if (!(*(tiles + i)) && i >= 0 && i < arraySize) {
            ammountOfVisibleTiles++;

            b2Vec2 center = b2Vec2(standardTileSize.x * positionInChunk.x
                                   , standardTileSize.y * positionInChunk.y);
            standardTileShape.SetAsBox(standardTileSize.x / 2.0f
                                       , standardTileSize.y / 2.0f
                                       , center, 0);
            standardTileDef.shape = &standardTileShape;
            *(fixtures + i) = chunkBody->CreateFixture(&standardTileDef);

            *(tiles + i) = tile;

            updateVertexArray();
        }
    }
}

char Chunk::getTile(Vector2i positionInChunk) {
    int i = positionInChunk.x + positionInChunk.y * chunkSize.x;
    return *(tiles + i);
}

void Chunk::moveInFocus() {
    if (!inFocus) {
        inFocus = true;
        chunkBody = world->CreateBody(&bodyDef);

        for (int i = 0; i < arraySize; i++) {
            *(this->tiles + i) = *(tiles + i);
            int x = i % chunkSize.x;
            int y = i / chunkSize.x;

            if (*(this->tiles + i)) { // tile is visible and solid
                b2Vec2 center = b2Vec2(standardTileSize.x * x
                                       , standardTileSize.y * y);
                standardTileShape.SetAsBox(standardTileSize.x / 2.0f
                                           , standardTileSize.y / 2.0f
                                           , center, 0);
                standardTileDef.shape = &standardTileShape;
                *(fixtures + i) = chunkBody->CreateFixture(&standardTileDef);
            } else {
                *(fixtures + i) = NULL;
            }
        }
    }
}

void Chunk::moveOutFocus() {
    if (inFocus) {
        inFocus = false;
        world->DestroyBody(chunkBody);
    } 
}

void Chunk::draw(RenderTarget &target, RenderStates states) const {
    states.transform *= getTransform();
    target.draw(tileQuads, states);
}

void Chunk::updateVertexArray() {
    tileQuads.resize(4 * ammountOfVisibleTiles);
    tileQuads.setPrimitiveType(Quads);

    int indexInVertexArray = 0;
    for (int i = 0; i < arraySize; i++) {
        int x = i % chunkSize.x;
        int y = i / chunkSize.x;
        if (*(this->tiles + i)) {
            int baseIndex = indexInVertexArray;

            /* Input the vertixes in the following order
             *  0 ___ 1
             *   |   |
             *   |___|
             *  3     2
             */
            tileQuads[baseIndex].position = Vector2f(x * tileSize.x
                                                     , y * tileSize.y);
            tileQuads[baseIndex+1].position = Vector2f((x + 1) * tileSize.x
                                                       , y * tileSize.y);
            tileQuads[baseIndex+2].position = Vector2f((x + 1) * tileSize.x
                                                       , (y + 1) * tileSize.y);
            tileQuads[baseIndex+3].position = Vector2f(x * tileSize.x
                                                       , (y + 1) * tileSize.y);

            Vector2f texCorner =
                Vector2f(((*(this->tiles + i) - 1) % tilesPerWidthTex)
                         * tileTexSize.x
                         , (*(this->tiles + i) / tilesPerWidthTex)
                         * tileTexSize.y);

            tileQuads[baseIndex].texCoords = texCorner;
            tileQuads[baseIndex+1].texCoords = texCorner + Vector2f(tileTexSize.x, 0);
            tileQuads[baseIndex+2].texCoords = texCorner + Vector2f(tileTexSize.x, tileTexSize.y);
            tileQuads[baseIndex+3].texCoords = texCorner + Vector2f(0, tileTexSize.y);

            indexInVertexArray += 4;
        }
    }
}
