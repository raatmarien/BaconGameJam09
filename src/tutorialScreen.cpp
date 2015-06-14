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

#include <tutorialScreen.h>
#include <string>

using namespace sf;

void TutorialScreen::initialize(Font *font) {
    this->font = font;
    padding = 30;
    tutorialText.setFont(*font);
    tutorialText.setCharacterSize(20);
    tutorialText.setPosition(2 * padding, 2 * padding);
    inReachText.setFont(*font);
    inReachText.setCharacterSize(20);
    inReachText.setString("Press 'e' to talk");
    window.setPosition(padding, padding);
    window.setFillColor(Color(40, 50, 100, 220));
    clickable = false;
}

void TutorialScreen::update() {
    if (active) {
        if (Keyboard::isKeyPressed(Keyboard::Return)) {
            if (clickable) {
                setScreen(currentScreen + 1);
                clickable = false;
            }
        } else {
            clickable = true;
        }
    }
}

void TutorialScreen::setScreenSize(int screenX, int screenY) {
    this->screenX = screenX;
    this->screenY = screenY;

    window.setSize(Vector2f(600, 600));
    window.setOrigin(window.getSize() / 2.0f);
    window.setPosition(Vector2f(screenX, screenY) / 2.0f);

    tutorialText.setPosition(window.getPosition() - (window.getSize() / 2.0f)
                     + Vector2f(padding, padding));

    inReachText.setPosition(padding * 4, screenY / 2 - padding * 2);
}

void TutorialScreen::startTutorial() {
    active = true;
    setScreen(1);
}

void TutorialScreen::draw(RenderTarget *target) {
    if (active) {
        target->draw(window);
        target->draw(tutorialText);
    } else if (inReach) {
        target->draw(inReachText);
    }
}

void TutorialScreen::setScreen(int screen) {
    currentScreen = screen;
    std::string tutorialString1 = "Controls:\n"
        "\t- To move: use wasd or the arrow keys\n"
        "\t- To use your jetpack: press space\n"
        "\t- To mine blocks: left click on tiles\n\n"
        "Press return to continue...";
    std::string tutorialString2 = "Hey, I'm Dave.\n\n"
        "I lost my precious diamond way down years ago,\n"
        "but I'm too old to retrieve it.\n"
        "I would reward you handsomely if you bring it back to me.\n\n"
        "Press return to continue...";
    std::string tutorialString3 = "Before you go down there though, let me give\n"
        "you a few tips:\n\n"
        "\t- Every ore you mine will give you money, you can\n"
        "\t  spend this money in the shop left from here\n"
        "\t- Deeper ores are worth more money, but the deeper you\n"
        "\t  the harder it will get to dig\n"
        "\t- Don't dig straight down, since you'll still have to\n"
        "\t  return to the surface eventually\n\n"
        "Press return to continue..";
    std::string tutorialString4 = "That was all, good luck finding my diamond!\n\n"
        "Press return to start digging...";
    switch (screen) {
    case 1:
        tutorialText.setString(tutorialString1);
        break;
    case 2:
        tutorialText.setString(tutorialString2);
        break;
    case 3:
        tutorialText.setString(tutorialString3);
        break;
    case 4:
        tutorialText.setString(tutorialString4);
        break;
    default:
        active = false;
        break;
    }
}
