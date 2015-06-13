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
#include <Box2D/Box2D.h>

#include <collidable.h>

struct PlayerSettings {
    sf::Vector2f size;
    sf::Vector2f startPosition;
    float moveForce;
    float jumpImpulse;
    float hitRadius;
    float standardDamage;
    sf::Texture *texture;
    int scale;
    b2World *world;
};

class Player : public sf::Drawable, public sf::Transformable {
public:
    void initialize(PlayerSettings settings);
    void update();
    void move(bool right);
    void jump();

    void addGroundContact();
    void removeGroundContact();

    float getHitRadius();
    float getDamage();
private:
    PlayerSettings settings;
    sf::Sprite sprite;
    b2Body *body;
    UserData userData;

    int groundContactsAmmount;

    virtual void draw(sf::RenderTarget &target
                      , sf::RenderStates states) const;

};
