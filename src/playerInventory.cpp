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

    moneyText.setFont(*moneyFont);
    moneyText.setCharacterSize(40);

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
    case 1:
        movementStrength = 15.0f;
        break;
    case 2:
        movementStrength = 20.0f;
        break;
    case 3:
        movementStrength = 30.0f;
        break;
    case 4:
        movementStrength = 45.0f;
        break;
    default:
        movementStrength = 45.0f + (movementLevel - 4) * 15.0f;
        break;
    }
    player->settings.moveForce = movementStrength;
}

int PlayerInventory::getJetpackLevel() {
    return jetpackLevel;
}

void PlayerInventory::levelupJetpack() {
    jetpackLevel++;

    float jetpackFuel = 300 + jetpackLevel * 200;
    // switch (jetpackLevel) {
    // case 1:
    //     break;
    // default:
    //     break;
    // }
    player->settings.maxFuel = jetpackFuel;
}

void PlayerInventory::setDiamond(bool hasDiamond) {
    diamond = hasDiamond;
}

bool PlayerInventory::getDiamond() {
    return diamond;
}

void PlayerInventory::draw(RenderTarget &target, RenderStates states) const {
    states.transform *= getTransform();
    target.draw(moneyText, states);
}
