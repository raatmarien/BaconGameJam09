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

#include <SFML/Graphics.hpp>
#include <player.h>

class CustomCursor : public sf::Drawable, public sf::Transformable {
public:
    void initialize(sf::Texture *texture, sf::Vector2i tileSize
                    , Player *player);
    void update(sf::Vector2i mousePosition);
private:
    sf::Sprite **tilePointers;
    Player *player;
    sf::Vector2i tileSize;
    
    virtual void draw(sf::RenderTarget &target
                      , sf::RenderStates states) const;

    float distance(sf::Vector2f vec1, sf::Vector2f vec2);
    sf::Vector2f roundTo(sf::Vector2i vecToRound, sf::Vector2i toRoundWith);
};
