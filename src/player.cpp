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
    groundContactsAmmount = 0;

    userData.type = COLLIDABLE_PLAYER;
    userData.owner = this;

    sprite.setTexture(*(this->settings.texture));
    sprite.setOrigin(this->settings.size.x / 2.0f
                     , this->settings.size.y / 2.0f);

    setPosition(this->settings.startPosition);

    // Setup body physics
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(this->settings.startPosition.x
                         / this->settings.scale
                         , this->settings.startPosition.y
                         / this->settings.scale);
    bodyDef.linearDamping = 4.0f;
    bodyDef.angularDamping = 1.0f;
    bodyDef.fixedRotation = true;
    body = this->settings.world->CreateBody(&bodyDef);

    float halfSizeX = this->settings.size.x
        / (2.0f * ((float) (this->settings.scale)));
    float halfSizeY = this->settings.size.y
        / (2.0f * ((float) (this->settings.scale)));
    b2PolygonShape polygon;
    /* Player shape:
           1___2
         0 /   \ 3
          |  .c |    c = center
          |     |
         7 \___/ 4
           6   5           
    */
    b2Vec2 vertices[8];
    vertices[0].Set(-1.0f * halfSizeX, -0.8f * halfSizeY);
    vertices[1].Set(-0.8f * halfSizeX, -1.0f * halfSizeY);
    vertices[2].Set(0.8f * halfSizeX, -1.0f * halfSizeY);
    vertices[3].Set(1.0f * halfSizeX, -0.8f * halfSizeY);
    vertices[4].Set(1.0f * halfSizeX, 0.8f * halfSizeY);
    vertices[5].Set(0.5f * halfSizeX, 1.0f * halfSizeY);
    vertices[6].Set(-0.5f * halfSizeX, 1.0f * halfSizeY);
    vertices[7].Set(-1.0f * halfSizeX, 0.8f * halfSizeY);

    b2Vec2 offset = b2Vec2(-8.0f / ((float) (this->settings.scale))
                           , -8.0f / ((float) (this->settings.scale)));
    for (int i = 0; i < 8; i++) vertices[i] += offset;
    
    polygon.Set(vertices, 8);
    
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &polygon;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.1f;

    body->CreateFixture(&fixtureDef);

    b2PolygonShape footSensorShape;
    footSensorShape.SetAsBox(0.5f * halfSizeX, 0.2f * halfSizeY
                             , b2Vec2(0, 0.9f * halfSizeY) + offset, 0);
    b2FixtureDef footFixtureDef;
    footFixtureDef.shape = &footSensorShape;
    footFixtureDef.isSensor = true;
    body->CreateFixture(&footFixtureDef)->SetUserData(&userData);
}

void Player::update() {
    b2Vec2 newPosition = body->GetPosition();
    setPosition(newPosition.x * settings.scale
                , newPosition.y * settings.scale);
}

void Player::move(bool right) {
    int direction = right ? 1 : -1;
    float groundMultiplier = groundContactsAmmount > 0 ?
        1 : 0.95f;
    body->ApplyForce(b2Vec2(direction * settings.moveForce * groundMultiplier
                            , 0)
                     , body->GetWorldCenter(), true);
}

void Player::jump() {
    if (groundContactsAmmount > 0) {
        body->ApplyLinearImpulse(b2Vec2(0, -settings.jumpImpulse)
                                 , body->GetWorldCenter(), true);
    }
}

void Player::down() {
    body->ApplyForce(b2Vec2(0, settings.downForce)
                     , body->GetWorldCenter(), true);
}

void Player::addGroundContact() {
    groundContactsAmmount++;
}

void Player::removeGroundContact() {
    groundContactsAmmount--;
}

float Player::getHitRadius() {
    return settings.hitRadius;
}

float Player::getDamage() {
    return settings.standardDamage;
}

void Player::draw(RenderTarget &target, RenderStates states) const {
    states.transform *= getTransform();
    target.draw(sprite, states);
}
