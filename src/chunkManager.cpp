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

void ChunkManager::hitTile(Vector2i globalPixelPosition, float damage) {
    int tileX = globalPixelPosition.x / settings.chunkSettings.tileSize.x;
    int tileY = globalPixelPosition.y / settings.chunkSettings.tileSize.y;
    int chunkX = tileX / settings.chunkSettings.chunkSize.x;
    int chunkY = tileY / settings.chunkSettings.chunkSize.y;
    int chunkI = chunkY * settings.worldSize.x + chunkX;    
    int inChunkX = tileX % settings.chunkSettings.chunkSize.x;
    int inChunkY = tileY % settings.chunkSettings.chunkSize.y;
    std::cout << chunkI << ": " << inChunkX << " " << inChunkY << "\n"; 
    chunks[chunkI]->removeTile(Vector2i(inChunkX, inChunkY));
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
        // Fill all the tiles
        for (int j = 0; j < tilesInChunk; j++) {
            tiles[i][j] = 1;
        }
    }

    // Fill the tiles
    // Vector2i tilesWorldSize =
    //     Vector2i(settings.worldSize.x
    //              * settings.chunkSettings.chunkSize.x
    //              , settings.worldSize.y
    //              * settings.chunkSettings.chunkSize.y);
    // std::cout << ammountOfChunks << " " << tilesInChunk << " " << tilesWorldSize.x
    //           <<  " " << tilesWorldSize.y << "\n";
    // for (int y = 0; y < tilesWorldSize.y; y++) {
    //     for (int x = 0; x < tilesWorldSize.x; x++) {
    //         if (y < settings.skySize) {
    //             setLocalTile(x, y, 0);
    //         } else {
    //             setLocalTile(x, y, 1);
    //         }
    //     }
    // }
}

void ChunkManager::setLocalTile(int x, int y, char tile) {
    int chunkX = x / settings.chunkSettings.chunkSize.x;
    int chunkY = y / settings.chunkSettings.chunkSize.y;
    int chunkI = chunkY * settings.worldSize.x + chunkX;

    int inChunkX = x % settings.chunkSettings.chunkSize.x;
    int inChunkY = y % settings.chunkSettings.chunkSize.y;
    int inChunkI = inChunkY * settings
        .chunkSettings.chunkSize.x + inChunkX;

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
