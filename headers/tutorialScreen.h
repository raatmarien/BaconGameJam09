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


class TutorialScreen {
public:
    void initialize(sf::Font *font);
    void update();
    void draw(sf::RenderTarget *target);
    void setScreenSize(int screenX, int screenY);
    void startTutorial();
    bool active;
    bool inReach;
private:
    sf::Font *font;
    sf::Text tutorialText, inReachText;
    sf::RectangleShape window;

    bool clickable;

    int screenX, screenY;
    int padding;
    int currentScreen;

    void setScreen(int screen);
};
