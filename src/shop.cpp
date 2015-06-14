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

#include <shop.h>
#include <math.h>
#include <iostream>

using namespace sf;

void Shop::initialize(Font *font, PlayerInventory *inventory) {
    this->font = font;
    this->inventory = inventory;
    padding = 30;

    items = new Text*[4];
    for (int i = 0; i < 4; i++) {
        items[i] = new Text;
        items[i]->setFont(*font);
        items[i]->setCharacterSize(20);
        float height = 50;
        items[i]->setPosition(padding * 2
                             , padding * (2 + i)
                             + height * i);
    }
    selector.setOutlineColor(Color::Green);
    selector.setOutlineThickness(5);
    selector.setFillColor(Color(0,0,0,0));
    inReachText.setFont(*font);
    inReachText.setCharacterSize(20);
    inReachText.setString("Press 'e' to shop");
    updateTexts();
    window.setPosition(padding, padding);
    window.setFillColor(Color(40, 50, 100, 220));
    clickableUp = false;
    clickableDown = false;
    clickableEnter = false;
}

void Shop::update() {
    if (active) {
        if (Keyboard::isKeyPressed(Keyboard::W)
            || Keyboard::isKeyPressed(Keyboard::Up)) {
            if (clickableUp) {
                currentItem--;
                updateCurrentItem();
                clickableUp = false;
            }
        } else {
            clickableUp = true;
        }
        if (Keyboard::isKeyPressed(Keyboard::S)
            || Keyboard::isKeyPressed(Keyboard::Down)) {
            if (clickableDown) {
                currentItem++;
                updateCurrentItem();
                clickableDown = false;
            }
        } else {
            clickableDown = true;
        }

        if (Keyboard::isKeyPressed(Keyboard::Return)) {
            if (clickableEnter) {
                int price;
                switch (currentItem) {
                case 0:
                    price = getPrice(inventory->getPickaxeLevel());
                    if (inventory->getMoney() > price) {
                        inventory->changeMoney(-price);
                        inventory->levelupPickaxe();
                    }
                    break;
                case 1:
                    price = getPrice(inventory->getMovementLevel());
                    if (inventory->getMoney() > price) {
                        inventory->changeMoney(-price);
                        inventory->levelupMovement();
                    }
                    break;
                case 2:
                    price = getPrice(inventory->getJetpackLevel());
                    if (inventory->getMoney() > price) {
                        inventory->changeMoney(-price);
                        inventory->levelupJetpack();
                    }
                    break;
                }
                updateTexts();
                clickableEnter = false;
            }
        } else {
            clickableEnter = true;
        }

        if (Keyboard::isKeyPressed(Keyboard::Escape)) {
            active = false;
        }
    }
}

void Shop::openShop() {
    active = true;
    currentItem = 0;
    updateCurrentItem();
}

void Shop::draw(RenderTarget *target) {
    if (active) {
        target->draw(window);
        for (int i = 0; i < 4; i++) {
            target->draw(*(items[i]));
        }
        target->draw(selector);
    } else if (inReach) {
        target->draw(inReachText);
    }
}

void Shop::setScreenSize(int screenX, int screenY) {
    this->screenX = screenX;
    this->screenY = screenY;

    window.setSize(Vector2f(600, 600));
    window.setOrigin(window.getSize() / 2.0f);
    window.setPosition(Vector2f(screenX, screenY) / 2.0f);

    inReachText.setPosition(padding * 4, screenY / 2 - padding * 2);

    float height = 50;
    for (int i = 0; i < 4; i++) {
        items[i]->setPosition(window.getPosition().x - (window.getSize().x / 2.0f)
                              + padding, padding * (2 + i) + height * i + window.getPosition().y
                              - (window.getSize().y / 2.0f));
    }
    updateCurrentItem();
    selector.setSize(Vector2f(window.getSize().x - padding * 2, height + padding));
}

void Shop::updateTexts() {
    ss << "Pickaxe strength:\t\t\t" << getPrice(inventory->getPickaxeLevel())
       << " gold"
       << "\nCurrent level: " << inventory->getPickaxeLevel();
    items[0]->setString(ss.str());
    ss.clear();
    ss.str("");
    ss << "Movement speed:\t\t\t" << getPrice(inventory->getMovementLevel())
       << " gold"
       << "\nCurrent level: " << inventory->getMovementLevel();
    items[1]->setString(ss.str());
    ss.clear();
    ss.str("");
    ss << "Jetpack fuel:\t\t\t\t\t" << getPrice(inventory->getJetpackLevel())
       << " gold"
       << "\nCurrent level: " << inventory->getJetpackLevel();
    items[2]->setString(ss.str());
    ss.clear();
    ss.str("");
    items[3]->setString("Use arrows to select and use return to purchase. Press 'esc' to quit");
}

void Shop::updateCurrentItem() {
    if (currentItem < 0) currentItem = 0;
    if (currentItem > 2) currentItem = 2;
    selector.setPosition(items[currentItem]->getPosition() - (Vector2f(padding, padding) / 2.0f));
}

int Shop::getPrice(int level) {
    switch (level) {
    case 1:
        return 5;
    case 2:
        return 10;
    case 3:
        return 25;
    case 4:
        return 50;
    case 5:
        return 80;
    case 6:
        return 110;
    default:
        return 110 * pow(1.5f, (float) (level - 6)); 
    }
}
