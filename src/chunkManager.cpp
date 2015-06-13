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

#include <chunkManager.h>
#include <iostream>

using namespace sf;

void ChunkManager::initialize(WorldSettings settings) {
    this->settings = settings;

    setupWorld();

    setupChunks();
}

void ChunkManager::hitTile(Vector2i globalPixelPosition, float damage
                           , Vector2f playerPosition, float hitRadius) {
    Vector2f positionRoundedToTile = Vector2f((int) (globalPixelPosition.x)
                                              , (int) (globalPixelPosition.y));
    positionRoundedToTile.x -= ((int) (positionRoundedToTile.x)) % settings.chunkSettings.tileSize.x; 
    positionRoundedToTile.y -= ((int) (positionRoundedToTile.y)) % settings.chunkSettings.tileSize.y; 
    positionRoundedToTile += Vector2f(settings.chunkSettings.tileSize.x / 2
                                      , settings.chunkSettings.tileSize.y / 2);
    playerPosition -= positionRoundedToTile;
    if (playerPosition.x * playerPosition.x + playerPosition.y * playerPosition.y
        > hitRadius * hitRadius)
        return;

    int tileX = globalPixelPosition.x / settings.chunkSettings.tileSize.x;
    int tileY = globalPixelPosition.y / settings.chunkSettings.tileSize.y;

    for (int i = 0; i < tileDamages.size(); i++) {
        if (tileDamages[i]->totalTileX == tileX
            && tileDamages[i]->totalTileY == tileY) {
            tileDamages[i]->currentStrength -= damage;
            if (tileDamages[i]->currentStrength < 0) {
                delete tileDamages[i];
                tileDamages.erase(tileDamages.begin() + i);
                // Destroy the tile
                int chunkX = tileX / settings.chunkSettings.chunkSize.x;
                int chunkY = tileY / settings.chunkSettings.chunkSize.y;
                int chunkI = chunkY * settings.worldSize.x + chunkX;    
                int inChunkX = tileX % settings.chunkSettings.chunkSize.x;
                int inChunkY = tileY % settings.chunkSettings.chunkSize.y;
                chunks[chunkI]->removeTile(Vector2i(inChunkX, inChunkY));
            }
            return;
        }
    }

    
    if (tileX >= 0 && tileX < settings.worldSize.x
        * settings.chunkSettings.chunkSize.x
        && tileY >= 0 && tileY < settings.worldSize.y
        * settings.chunkSettings.chunkSize.y) {
        int chunkX = tileX / settings.chunkSettings.chunkSize.x;
        int chunkY = tileY / settings.chunkSettings.chunkSize.y;
        int chunkI = chunkY * settings.worldSize.x + chunkX;    
        int inChunkX = tileX % settings.chunkSettings.chunkSize.x;
        int inChunkY = tileY % settings.chunkSettings.chunkSize.y;
        float tileStrength =  getTileStrength(
            chunks[chunkI]->getTile(Vector2i(inChunkX, inChunkY)));
        if (tileStrength < damage) {
            chunks[chunkI]->removeTile(Vector2i(inChunkX, inChunkY));
            return;
        }

        TileDamage *tileDamage = new TileDamage;
        tileDamage->totalTileX = tileX;
        tileDamage->totalTileY = tileY;
        tileDamage->maxStrength = tileStrength;
        tileDamage->currentStrength = tileDamage->maxStrength - damage;

        tileDamages.push_back(tileDamage);
    }
}

void ChunkManager::update(View *currentView) {
    Vector2f viewCenter = currentView->getCenter();
    Vector2f viewSize = currentView->getSize();
    IntRect viewRect = IntRect(viewCenter.x - (viewSize.x / 2.0f)
                               , viewCenter.y - (viewSize.y / 2.0f)
                               , viewSize.x
                               , viewSize.y);
    for (int i = 0; i < ammountOfChunks; i++) {
        if (chunks[i]->chunkRect.intersects(viewRect)) {
            chunks[i]->moveInFocus();
        } else {
            chunks[i]->moveOutFocus();
        }
    }

    // Update tileDamages
    for (int i = 0; i < tileDamages.size(); i++) {
        tileDamages[i]->currentStrength *= 1.02f;
        if (tileDamages[i]->currentStrength >= tileDamages[i]->maxStrength) {
            delete tileDamages[i];
            tileDamages.erase(tileDamages.begin() + i);
        }
    }
}

void ChunkManager::draw(RenderTarget *target) {
    View currentView = target->getView();
    Vector2f viewCenter = currentView.getCenter();
    Vector2f viewSize = currentView.getSize();
    IntRect viewRect = IntRect(viewCenter.x - (viewSize.x / 2.0f)
                               , viewCenter.y - (viewSize.y / 2.0f)
                               , viewSize.x
                               , viewSize.y);
    for (int i = 0; i < ammountOfChunks; i++) {
        if (chunks[i]->chunkRect.intersects(viewRect)) {
            target->draw(*(chunks[i]), settings.chunkTexture);
        }
    }
}

void ChunkManager::setupWorld() {
    // Set up the tiles array
    ammountOfChunks = settings.worldSize.x * settings.worldSize.y;
    tiles = new char*[ammountOfChunks];
    tilesInChunk = settings.chunkSettings.chunkSize.x
        * settings.chunkSettings.chunkSize.y;
    for (int i = 0; i < ammountOfChunks; i++) {
        tiles[i] = new char[tilesInChunk];
    }

    Vector2i tilesWorldSize =
        Vector2i(settings.worldSize.x
                 * settings.chunkSettings.chunkSize.x
                 , settings.worldSize.y
                 * settings.chunkSettings.chunkSize.y);
    for (int y = 0; y < tilesWorldSize.y; y++) {
        for (int x = 0; x < tilesWorldSize.x; x++) {
            if (y) 
                setLocalTile(x, y, 1);
            else
                setLocalTile(x, y, 2);
        }
    }

    tilesInHeight = settings.worldSize.y * settings.chunkSettings.chunkSize.y;
    tilesInWidth = settings.worldSize.x * settings.chunkSettings.chunkSize.x;

    OrePlaceSettings coalSettings;
    coalSettings.minY = 1;
    coalSettings.maxY = 100;
    coalSettings.chanceN = 10000;
    coalSettings.baseChanceT = 7500;
    coalSettings.yChanceT = 25;
    coalSettings.triesPerY = 19;
    coalSettings.tilesTries = 15;
    coalSettings.tilesSpread = 4;
    coalSettings.tile = 3;

    addOre(coalSettings);

    OrePlaceSettings copperSettings;
    copperSettings.minY = 10;
    copperSettings.maxY = 160;
    copperSettings.chanceN = 10000;
    copperSettings.baseChanceT = 10000;
    copperSettings.yChanceT = -20;
    copperSettings.triesPerY = 19;
    copperSettings.tilesTries = 15;
    copperSettings.tilesSpread = 4;
    copperSettings.tile = 4;

    addOre(copperSettings);
}

void ChunkManager::setLocalTile(int x, int y, char tile) {
    int chunkX = x / settings.chunkSettings.chunkSize.x;
    int chunkY = y / settings.chunkSettings.chunkSize.y;
    int chunkI = chunkY * settings.worldSize.x + chunkX;

    int inChunkX = x % settings.chunkSettings.chunkSize.x;
    int inChunkY = y % settings.chunkSettings.chunkSize.y;
    int inChunkI = inChunkY * settings
        .chunkSettings.chunkSize.x + inChunkX;

    // std::cout << chunkI << " " << inChunkI << "\n";
    tiles[chunkI][inChunkI] = tile;
}

void ChunkManager::setupChunks() {
    chunks = new Chunk*[ammountOfChunks];
    for (int i = 0; i < ammountOfChunks; i++) {
        int x = i % settings.worldSize.x;
        int y = i / settings.worldSize.x;
        Vector2f position = Vector2f(x * settings.chunkSettings.tileSize.x
                                     * settings.chunkSettings.chunkSize.x
                                     , y * settings.chunkSettings.tileSize.y
                                     * settings.chunkSettings.chunkSize.y);
        settings.chunkSettings.position = position;
        Chunk *chunk = new Chunk(&(settings.chunkSettings), tiles[i]);
        chunks[i] = chunk;
    }
}

void ChunkManager::addOre(OrePlaceSettings settings) {
    for (int y = settings.minY; y < tilesInHeight && y < settings.maxY; y++) {
        for (int i = 0; i < settings.triesPerY; i++) {
            if ((rand() % settings.chanceN) > settings.baseChanceT
                + (y - settings.minY) * settings.yChanceT) {
                int ammountTiles = rand() % settings.tilesTries;
                Vector2i basePos = Vector2i(rand() % tilesInWidth, y);
                for (int j = 0; j < ammountTiles; j++) {
                    setLocalTile(basePos.x + (rand() % settings.tilesSpread)
                                 , basePos.y + (rand() % settings.tilesSpread), settings.tile);
                }
            }
        }
    }
}

float ChunkManager::getTileStrength(char tile) {
    switch (tile) {
    case 0:
        return 0;
    case 1:
        return 1.0f;
    case 2:
        return 10000000000.0f;
    case 3:
        return 2.0f;
    }
    return 10.0f;
}
