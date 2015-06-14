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
#include <sstream>

class PlayerInventory : public sf::Drawable, public sf::Transformable {
public:
    void initialize(Player *player, sf::Font *moneyFont);
    void update();
    void changeMoney(int change);
    int getMoney();
    int getPickaxeLevel();
    void levelupPickaxe();
    int getMovementLevel();
    void levelupMovement();
    int getJetpackLevel();
    void levelupJetpack();
    void setDiamond(bool hasDiamond);
    bool getDiamond();
private:
    Player *player;

    int money;
    int pickaxeLevel;
    int movementLevel;
    int jetpackLevel;
    bool diamond;
    
    std::stringstream sstream;

    sf::Font *font;
    sf::Text moneyText;
    sf::RectangleShape fuelIn, fuelOutline;

    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
