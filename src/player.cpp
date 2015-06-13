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

#include <player.h>
#include <iostream>

using namespace sf;

void Player::initialize(PlayerSettings settings) {
    this->settings = settings;

    sprite.setTexture(*(this->settings.texture));
    sprite.setOrigin(this->settings.size / 2.0f);

    setPosition(this->settings.startPosition);

    // Setup body physics
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(this->settings.startPosition.x
                         / this->settings.scale
                         , this->settings.startPosition.y
                         / this->settings.scale);
    bodyDef.linearDamping = 0.5f;
    bodyDef.angularDamping = 1.0f;
    bodyDef.fixedRotation = true;
    body = this->settings.world->CreateBody(&bodyDef);

    float halfSizeY = this->settings.size.x
        / (2.0f * ((float) (this->settings.scale)));
    float halfSizeX = this->settings.size.y
        / (2.0f * ((float) (this->settings.scale)));
    std::cout << halfSizeX << " " << halfSizeY << "\n";
    b2PolygonShape polygon;
    /* Player shape:
           5___4
         6 /   \ 3
          |     |
          |     |
         7 \___/ 2
           0   1           
    */
    b2Vec2 vertices[8];
    vertices[0].Set(-0.5f * halfSizeX, 1.0f * halfSizeY);
    vertices[1].Set(0.5f * halfSizeX, 1.0f * halfSizeY);
    vertices[2].Set(1.0f * halfSizeX, 0.6f * halfSizeY);
    vertices[3].Set(1.0f * halfSizeX, -0.8f * halfSizeY);
    vertices[4].Set(0.8f * halfSizeX, -1.0f * halfSizeY);
    vertices[5].Set(-0.8f * halfSizeX, -1.0f * halfSizeY);
    vertices[6].Set(-1.0f * halfSizeX, -0.8f * halfSizeY);
    vertices[7].Set(-1.0f * halfSizeX, 0.6f * halfSizeY);
    
    polygon.Set(vertices, 8);
    
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &polygon;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 1.0f;

    body->CreateFixture(&fixtureDef);
}

void Player::update() {
    b2Vec2 newPosition = body->GetPosition();
    setPosition(newPosition.x * settings.scale
                , newPosition.y * settings.scale);
}

void Player::move(bool right) {
    int direction = right ? 1 : -1;
    body->ApplyForce(b2Vec2(direction * settings.moveForce, 0)
                     , body->GetWorldCenter(), true);
}

void Player::jump() {
    body->ApplyLinearImpulse(b2Vec2(0, -settings.jumpImpulse)
                             , body->GetWorldCenter(), true);
}

void Player::draw(RenderTarget &target, RenderStates states) const {
    states.transform *= getTransform();
    target.draw(sprite, states);
}
