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

#include <customCursor.h>
#include <math.h>
#include <iostream>

using namespace sf;

void CustomCursor::initialize(Texture *texture, Vector2i tileSize
                              , Player *player) {
    this->player = player;
    this->tileSize = tileSize;
    
    tilePointers = new Sprite*[4];

    for (int i = 0; i < 4; i++) {
        tilePointers[i] = new Sprite;
        tilePointers[i]->setTexture(*texture);
    }
    tilePointers[0]->setTextureRect(IntRect(0, 0, 16, 16));
    tilePointers[1]->setTextureRect(IntRect(16, 0, 16, 16));
    tilePointers[2]->setTextureRect(IntRect(16, 16, 16, 16));
    tilePointers[3]->setTextureRect(IntRect(0, 16, 16, 16));
}

void CustomCursor::update(Vector2i mousePosition) {
    Vector2i dif = tileSize / 2;
    tilePointers[0]->setPosition(
        roundTo(mousePosition + Vector2i(-dif.x, -dif.y), tileSize));
    tilePointers[1]->setPosition(
        roundTo(mousePosition + Vector2i(dif.x, -dif.y), tileSize));
    tilePointers[2]->setPosition(
        roundTo(mousePosition + Vector2i(dif.x, dif.y), tileSize));
    tilePointers[3]->setPosition(
        roundTo(mousePosition + Vector2i(-dif.x, dif.y), tileSize));

    for (int i = 0; i < 4; i++) {
        if (distance(tilePointers[i]->getPosition() + Vector2f(tileSize.x / 2
                                                              , tileSize.y / 2)
                     , player->getPosition()) < player->getHitRadius()) {
            tilePointers[i]->setColor(Color(0, 150, 0, 255));
        } else {
            tilePointers[i]->setColor(Color(150, 0, 0, 255));
        }
    }
}

void CustomCursor::draw(RenderTarget &target, RenderStates states) const {
    states.transform *= getTransform();
    for (int i = 0; i < 4; i++) {
        target.draw(*(tilePointers[i]), states);
    }
}

float CustomCursor::distance(Vector2f vec1, Vector2f vec2) {
    Vector2f dif = vec1 - vec2;
    float dist = sqrt(dif.x * dif.x + dif.y * dif.y);
    return dist;
}

Vector2f CustomCursor::roundTo(Vector2i vecToRound, Vector2i toRoundWith) {
    vecToRound.x -= vecToRound.x % toRoundWith.x;
    vecToRound.y -= vecToRound.y % toRoundWith.y;
    return Vector2f(vecToRound.x, vecToRound.y);
}
