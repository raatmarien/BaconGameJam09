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

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <iostream>

#include <chunk.h>
#include <chunkManager.h>
#include <player.h>
#include <contactListener.h>

#define SCALE 48

using namespace sf;

void loadFiles();
void handleEvents(RenderWindow *window);
void handleInput(RenderWindow *window);
void update();
void simulatePhysics();
void draw(RenderWindow *window);

// Generic global variables
int screenSizeX = 1920, screenSizeY = 1080;

// Textures
Texture blocksTexture, playerTexture;

// Game objects
ChunkManager chunkManager;

Player player;

ContactListener contactListener;

View view;

// Box2D settings
b2Vec2 gravity(0.0f, 20.0f);
b2World world(gravity);

float box2DTimeStep = 1.0f / 60.0f;
int velocityIterations = 8
    , positionIterations = 3;

Clock fpsTimer;

int main() {
    RenderWindow window(VideoMode(screenSizeX, screenSizeY), "Chunk test");
    window.setVerticalSyncEnabled(true);

    loadFiles();

    // Setup world
    world.SetContactListener(&contactListener);

    // Setup ChunkManager
    ChunkSettings chunkSettings;
    chunkSettings.chunkSize = Vector2i(64, 64);
    chunkSettings.tileSize = Vector2i(16, 16);
    chunkSettings.tileTexSize = Vector2f(16, 16);
    chunkSettings.tilesPerWidthTex = 100;
    chunkSettings.scale = SCALE;
    chunkSettings.world = &world;

    WorldSettings worldSettings;
    worldSettings.worldSize = Vector2i(20, 40);
    worldSettings.chunkTexture = &blocksTexture;
    worldSettings.chunkSettings = chunkSettings;

    chunkManager.initialize(worldSettings);

    // Setup Player
    PlayerSettings playerSettings;
    playerSettings.size = Vector2f(28, 48);
    playerSettings.startPosition = Vector2f(1000, -100 - (playerSettings.size.y / 2));
    playerSettings.moveForce = 15.0f;
    playerSettings.jumpImpulse = 5.0f;
    playerSettings.texture = &playerTexture;
    playerSettings.scale = SCALE;
    playerSettings.world = &world;

    player.initialize(playerSettings);

    // Setup View
    view.setCenter(0, 1080);
    view.setSize(screenSizeX, screenSizeY);

    int framesForFps = -1;
    fpsTimer.restart();
    while (window.isOpen()) {
        if (framesForFps == 60) {
            float time = fpsTimer.restart().asSeconds();
            std::cout << "FPS: " << (int) (1.0f / (time / 60.0f) + 0.5f) << "\n";
            framesForFps = 0;
        }
        framesForFps++;

        handleEvents(&window);
        handleInput(&window);
        update();
        simulatePhysics();
        draw(&window);
    }
}

void handleEvents(RenderWindow *window) {
    Event event;
    while (window->pollEvent(event)) {
        if (event.type == Event::Closed) {
            window->close();
        }
        if (event.type == Event::Resized) {
            screenSizeX = event.size.width;
            screenSizeY = event.size.height;
            std::cout << "New screen size: " << screenSizeX
                      << " by " << screenSizeY << "\n";
            view.setSize(screenSizeX, screenSizeY);
        }
    }
}

void handleInput(RenderWindow *window) {
    // Mouse input
    if (Mouse::isButtonPressed(Mouse::Left)) {
        Vector2i globalMousePosition = Mouse::getPosition(*window)
            + Vector2i(view.getCenter().x - (screenSizeX / 2)
                       , view.getCenter().y - (screenSizeY / 2));
        chunkManager.hitTile(globalMousePosition + Vector2i(-8, -8), 1);
        chunkManager.hitTile(globalMousePosition + Vector2i(-8, 8), 1);
        chunkManager.hitTile(globalMousePosition + Vector2i(8, 8), 1);
        chunkManager.hitTile(globalMousePosition + Vector2i(8, -8), 1);
    }
    if (Mouse::isButtonPressed(Mouse::Right)) {

    }

    // Keyboard input
    if (Keyboard::isKeyPressed(Keyboard::W)
        || Keyboard::isKeyPressed(Keyboard::Up)) {
        player.jump();
    }
    if (Keyboard::isKeyPressed(Keyboard::S)
        || Keyboard::isKeyPressed(Keyboard::Down)) {
    }
    if (Keyboard::isKeyPressed(Keyboard::A)
        || Keyboard::isKeyPressed(Keyboard::Left)) {
        player.move(false);
    }
    if (Keyboard::isKeyPressed(Keyboard::D)
        || Keyboard::isKeyPressed(Keyboard::Right)) {
        player.move(true);
    }
}

void update() {
    chunkManager.update(&view);
    player.update();
    view.setCenter(player.getPosition());
}

void simulatePhysics() {
    world.Step(box2DTimeStep, velocityIterations
               , positionIterations);
    world.ClearForces();
}

void draw(RenderWindow *window) {
    window->setView(view);
    window->clear(Color(20, 50, 200));
    chunkManager.draw(window);
    window->draw(player);
    window->display();
}

void loadFiles() {
    blocksTexture.loadFromFile("sprites/blocksTexture.png");
    playerTexture.loadFromFile("sprites/player.png");
}
