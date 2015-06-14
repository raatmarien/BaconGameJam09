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

#include <playerInventory.h>
#include <string>
#include <iostream>

using namespace sf;

void PlayerInventory::initialize(Player *player, Font *moneyFont) {
    this->player = player;

    diamond = false;

    float padding = 2;

    moneyText.setFont(*moneyFont);
    moneyText.setCharacterSize(40);
    fuelIn.setPosition(padding, padding * 2 + 60);
    fuelOutline.setPosition(fuelIn.getPosition());
    fuelIn.setFillColor(Color::Green);
    fuelOutline.setFillColor(Color(0,0,0,0));
    fuelOutline.setOutlineColor(Color::White);
    fuelOutline.setOutlineThickness(2);
    fuelOutline.setSize(Vector2f(150, 20));

    money = 0;

    sstream << "Money: " << money;
    std::string moneyStr = sstream.str();
    sstream.clear();
    sstream.str("");
    moneyText.setString(moneyStr);

    pickaxeLevel = 0;
    levelupPickaxe();
    movementLevel = 0;
    levelupMovement();
    jetpackLevel = 0;
    levelupJetpack();
}

void PlayerInventory::changeMoney(int change) {
    money += change;
    sstream << "Money: " << money;
    std::string moneyStr = sstream.str();
    sstream.clear();
    sstream.str("");
    moneyText.setString(moneyStr);
}

int PlayerInventory::getMoney() {
    return money;
}

int PlayerInventory::getPickaxeLevel() {
    return pickaxeLevel;
}

void PlayerInventory::levelupPickaxe() {
    pickaxeLevel++;

    float pickaxeStrength = 0.0f;
    switch (pickaxeLevel) {
    case 1:
        pickaxeStrength = 0.05f;
        break;
    case 2:
        pickaxeStrength = 0.08f;
        break;
    case 3:
        pickaxeStrength = 0.12f;
        break;
    case 4:
        pickaxeStrength = 0.2f;
        break;
    default:
        pickaxeStrength = 0.1f * (pickaxeLevel - 4) + 0.2f;
        break;
    }
    player->settings.standardDamage = pickaxeStrength;
}

int PlayerInventory::getMovementLevel() {
    return movementLevel;
}

void PlayerInventory::levelupMovement() {
    movementLevel++;

    float movementStrength = 0.0f;
    switch (movementLevel) {
    default:
        movementStrength = 15.0f + (movementLevel - 1) * 1.0f;
        break;
    }
    player->settings.moveForce = movementStrength;
}

int PlayerInventory::getJetpackLevel() {
    return jetpackLevel;
}

void PlayerInventory::levelupJetpack() {
    jetpackLevel++;

    int jetpackFuel = 20 + jetpackLevel * 20;
    float fuelAdd = 0.25f;
    float jetpackStrength = 38.8f + 1.2f * jetpackLevel;
    // switch (jetpackLevel) {
    // case 1:
    //     break;
    // default:
    //     break;
    // }
    player->maxFuel = jetpackFuel;
    player->fuelAdd = fuelAdd;
    player->jetpackStrength = jetpackStrength;
}

void PlayerInventory::setDiamond(bool hasDiamond) {
    diamond = hasDiamond;
}

bool PlayerInventory::getDiamond() {
    return diamond;
}

void PlayerInventory::update() {
    fuelIn.setSize(Vector2f(150.0f * (player->currentFuelFloat / (float) (player->maxFuel)), 20)); 
}

void PlayerInventory::draw(RenderTarget &target, RenderStates states) const {
    states.transform *= getTransform();
    target.draw(moneyText, states);
    target.draw(fuelIn, states);
    target.draw(fuelOutline, states);
}
