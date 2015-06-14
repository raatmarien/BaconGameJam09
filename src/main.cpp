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
#include <customCursor.h>
#include <playerInventory.h>
#include <tutorialScreen.h>
#include <shop.h>
#include <stdlib.h>

#define SCALE 48

using namespace sf;

void loadFiles();
void setWorldBoundaries(int width, int depth);

void handleEvents(RenderWindow *window);
void handleInput(RenderWindow *window);
void update(RenderWindow *window);
void simulatePhysics();
void draw(RenderWindow *window);

// Generic global variables
int screenSizeX = 1920, screenSizeY = 1080;
int worldSizeX, worldSizeY;

// Textures
Texture blocksTexture, playerTexture, cursorTexture
                                       , daveTexture;

Font moneyFont;

// Game objects
ChunkManager chunkManager;

Player player;

ContactListener contactListener;

CustomCursor customCursor;

PlayerInventory playerInventory;

TutorialScreen tutorialScreen;

Sprite dave;

Shop shop;

View view, UIView;

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
    window.setMouseCursorVisible(false);

    loadFiles();

    srand(time(NULL));

    // Setup world
    world.SetContactListener(&contactListener);

    ChunkSettings chunkSettings;
    chunkSettings.chunkSize = Vector2i(64, 64);
    chunkSettings.tileSize = Vector2i(16, 16);
    chunkSettings.tileTexSize = Vector2f(16, 16);
    chunkSettings.tilesPerWidthTex = 4;
    chunkSettings.backgroundColor = Color(62, 42, 29);
    chunkSettings.scale = SCALE;
    chunkSettings.world = &world;

    WorldSettings worldSettings;
    worldSettings.worldSize = Vector2i(10, 6);
    worldSettings.chunkTexture = &blocksTexture;
    worldSettings.chunkSettings = chunkSettings;


    worldSizeX = chunkSettings.chunkSize.x * chunkSettings.tileSize.x * worldSettings.worldSize.x;
    worldSizeY = chunkSettings.chunkSize.y * chunkSettings.tileSize.y * worldSettings.worldSize.y;

    setWorldBoundaries(worldSizeX, worldSizeY);

    // Setup Player
    PlayerSettings playerSettings;
    playerSettings.size = Vector2f(28, 48);
    playerSettings.startPosition = Vector2f((chunkSettings.chunkSize.x * chunkSettings.tileSize.x
                                             * worldSettings.worldSize.x) / 2
                                            ,  - (playerSettings.size.y / 2) + 16);
    playerSettings.moveForce = 15.0f;
    playerSettings.jumpImpulse = 5.0f;
    playerSettings.downForce = 20.0f;
    playerSettings.hitRadius = ((float) chunkSettings.tileSize.x) * 5.0f;
    playerSettings.standardDamage = 0.05f;
    playerSettings.texture = &playerTexture;
    playerSettings.scale = SCALE;
    playerSettings.world = &world;

    player.initialize(playerSettings);

    // Setup PlayerInventory
    playerInventory.initialize(&player, &moneyFont);
    playerInventory.setPosition(20, 20);

    // Setup ChunkManager
    worldSettings.inventory = &playerInventory;
    chunkManager.initialize(worldSettings);

    // Setup CustomCursor
    customCursor.initialize(&cursorTexture, chunkSettings.tileSize, &player);

    // Set up TutorialScreen
    tutorialScreen.initialize(&moneyFont);
    tutorialScreen.setScreenSize(screenSizeX, screenSizeY);
    tutorialScreen.startTutorial();

    // Set up dave
    dave.setTexture(daveTexture);
    dave.setPosition(playerSettings.startPosition - Vector2f(48, 24));

    // Set up shop
    shop.initialize(&moneyFont, &(playerInventory));
    shop.setScreenSize(screenSizeX, screenSizeY);

    // Setup View
    view.setCenter(0, 1080);
    view.setSize(screenSizeX, screenSizeY);
    UIView.setSize(screenSizeX, screenSizeY);
    UIView.setCenter(screenSizeX / 2, screenSizeY / 2);

    int framesForFps = -1;
    fpsTimer.restart();
    while (window.isOpen()) {
        if (framesForFps == 60) {
            float time = fpsTimer.restart().asSeconds();
            std::cout << "FPS: " << (int) (1.0f / (time / 60.0f) + 0.5f) << "\n";
            framesForFps = 0;
        }
        framesForFps++;

        if (tutorialScreen.active) {
            handleEvents(&window);
            update(&window);
            tutorialScreen.update();
            draw(&window);
            window.setView(UIView);
            window.display();
        } else if (shop.active) {
            handleEvents(&window);
            update(&window);
            shop.update();
            draw(&window);
            window.setView(UIView);
            window.display();
        } else {
            handleEvents(&window);
            handleInput(&window);
            update(&window);
            simulatePhysics();
            draw(&window);
            window.display();
        }
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
            UIView.setSize(screenSizeX, screenSizeY);
            UIView.setCenter(screenSizeX / 2, screenSizeY / 2);
            tutorialScreen.setScreenSize(screenSizeX, screenSizeY);
            shop.setScreenSize(screenSizeX, screenSizeY);
        }
    }
}

void handleInput(RenderWindow *window) {
    // Mouse input
    if (Mouse::isButtonPressed(Mouse::Left)) {
        Vector2i globalMousePosition = Mouse::getPosition(*window)
            + Vector2i(view.getCenter().x - (screenSizeX / 2)
                       , view.getCenter().y - (screenSizeY / 2));
        chunkManager.hitTile(globalMousePosition + Vector2i(-8, -8), player.getDamage()
                             , player.getPosition(), player.getHitRadius());
        chunkManager.hitTile(globalMousePosition + Vector2i(-8, 8), player.getDamage()
                             , player.getPosition(), player.getHitRadius());
        chunkManager.hitTile(globalMousePosition + Vector2i(8, 8), player.getDamage()
                             , player.getPosition(), player.getHitRadius());
        chunkManager.hitTile(globalMousePosition + Vector2i(8, -8), player.getDamage()
                             , player.getPosition(), player.getHitRadius());
    }
    if (Mouse::isButtonPressed(Mouse::Right)) {
        // TODO remove
        shop.openShop();
    }

    // Keyboard input
    if (Keyboard::isKeyPressed(Keyboard::W)
        || Keyboard::isKeyPressed(Keyboard::Up)
        || Keyboard::isKeyPressed(Keyboard::Space)) {
        player.jump();
    }
    if (Keyboard::isKeyPressed(Keyboard::S)
        || Keyboard::isKeyPressed(Keyboard::Down)) {
        player.down();
    }
    if (Keyboard::isKeyPressed(Keyboard::A)
        || Keyboard::isKeyPressed(Keyboard::Left)) {
        player.move(false);
    }
    if (Keyboard::isKeyPressed(Keyboard::D)
        || Keyboard::isKeyPressed(Keyboard::Right)) {
        player.move(true);
    }

    if (tutorialScreen.inReach
        && Keyboard::isKeyPressed(Keyboard::E)) {
        tutorialScreen.startTutorial();
    }
}

void update(RenderWindow *window) {
    chunkManager.update(&view);
    player.update();
    customCursor.update(Mouse::getPosition(*window)
            + Vector2i(view.getCenter().x - (screenSizeX / 2)
                       , view.getCenter().y - (screenSizeY / 2)));
    view.setCenter(player.getPosition());

    // Correct view position
    Vector2f viewCenter = view.getCenter();
    if (viewCenter.x < (screenSizeX / 2))
        view.setCenter(Vector2f((screenSizeX / 2), viewCenter.y));
    else if (viewCenter.x > worldSizeX - (screenSizeX / 2))
        view.setCenter(Vector2f(worldSizeX - (screenSizeX / 2), viewCenter.y));
    viewCenter = view.getCenter();
    if (viewCenter.y > worldSizeY - (screenSizeY / 2))
        view.setCenter(viewCenter.x, worldSizeY - (screenSizeY / 2));

    // Check if in tutorials reach
    Vector2f dif = player.getPosition() - dave.getPosition();
    if (dif.x * dif.x + dif.y * dif.y < 100 * 100) {
        tutorialScreen.inReach = true;
    } else {
        tutorialScreen.inReach = false;
    }
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
    window->draw(dave);
    window->draw(player);
    window->draw(customCursor);
    window->setView(UIView);
    window->draw(playerInventory);
    if (tutorialScreen.inReach || tutorialScreen.active) {
        tutorialScreen.draw(window);
    } else if (shop.inReach || shop.active) {
        shop.draw(window);
    }
}

void loadFiles() {
    blocksTexture.loadFromFile("sprites/blocksTexture.png");
    playerTexture.loadFromFile("sprites/player.png");
    daveTexture.loadFromFile("sprites/dave.png");
    cursorTexture.loadFromFile("sprites/cursor.png");
    moneyFont.loadFromFile("fonts/Munro.ttf");
}

void setWorldBoundaries(int width, int depth) {
    float heightAbove = 1000;
    float boundaryThickness = 16;
    b2BodyDef boundaryBodyDef;
    boundaryBodyDef.type = b2_staticBody;
    boundaryBodyDef.position.Set(0, 0);

    b2Body *boundaryBody = world.CreateBody(&boundaryBodyDef);

    b2PolygonShape edgeShape;
    edgeShape.SetAsBox((boundaryThickness / (float) (2 * SCALE))
                       , (float) (heightAbove + depth) / (float) (2 * SCALE)
                       , b2Vec2(-1.0f * ((boundaryThickness) / (float) (2 * SCALE))
                                , (float) (depth) / (float) (2 * SCALE))
                       , 0);
    b2FixtureDef edgeFixtureDef;
    edgeFixtureDef.shape = &edgeShape;

    boundaryBody->CreateFixture(&edgeFixtureDef);

    edgeShape.SetAsBox((boundaryThickness / (float) (2 * SCALE))
                       , (float) (heightAbove + depth) / (float) (2 * SCALE)
                       , b2Vec2(((boundaryThickness + (2 * (float) (width))) / (float) (2 * SCALE))
                                , (float) (depth) / (float) (2 * SCALE))
                       , 0);

    boundaryBody->CreateFixture(&edgeFixtureDef);

    edgeShape.SetAsBox((float) (width) / (float) (2 * SCALE)
                       , (boundaryThickness / (float) (2 * SCALE))
                       , b2Vec2((float) (width) / (float) (2 * SCALE)
                                , ((boundaryThickness + (2 * (float) (depth))) / (float) (2 * SCALE)))
                       , 0);

    boundaryBody->CreateFixture(&edgeFixtureDef);
}
