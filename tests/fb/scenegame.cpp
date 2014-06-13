#include <iostream>

#include "scenegame.hpp"
#include "menus.hpp"
#include "helpers.hpp"

using namespace sqt;

SceneGame::SceneGame(sq::Application* _app) : sq::Scene(_app) {
    seed = 123456789;

    tickRate = 120;
    dt = 1/120.d;

    sSpeed = 2;
    pVel = 0.f;

    pX = 60.f;
    pY = 300.f;

    flap = 0;
    flapCount = 0;

    dist = 0;
    newPipeCount = 0;

    setup_game(1000);
}

void SceneGame::update() {
    pRot = atan2(pVel, sSpeed);
    dist += sSpeed;

    pY += pVel;
    pVel += 0.08f;

    flapCount += abs(pVel);
    if (flapCount > 64) flapCount = 0;
    if      (flapCount > 48) flap = 1;
    else if (flapCount > 32) flap = 2;
    else if  (flapCount > 16) flap = 1;
    else                      flap = 0;

    newPipeCount += 1;
    if (newPipeCount == 60) {
        newPipeCount = 0;
        add_pipe(seed);
    }
}

void SceneGame::render(sf::RenderTarget& target, float ft) {
    static sf::Sprite flappy(app->texHolder.get_texture("flappy"));
    static sf::Sprite floor(app->texHolder.get_texture("floor"));
    static sf::Sprite background(app->texHolder.get_texture("background"));
    static sf::Sprite captop(app->texHolder.get_texture("captop"));
    static sf::Sprite capbottom(app->texHolder.get_texture("capbottom"));
    static sf::Sprite pipe(app->texHolder.get_texture("pipe"));
    static bool first = true;
    if (first) {
        flappy.setScale(4.f, 4.f);
        floor.setScale(4.f, 4.f);
        background.setScale(4.f, 4.f);
        captop.setScale(4.f, 4.f);
        capbottom.setScale(4.f, 4.f);
        pipe.setScale(4.f, 4.f);

        flappy.setTextureRect({17, 0, 17, 12});
        flappy.setOrigin(8.5f, 6.f);

        first = false;
    }

    if      (flap == 1) flappy.setTextureRect({17, 0, 17, 12});
    else if (flap == 0) flappy.setTextureRect({0, 0, 17, 12});
    else if (flap == 2) flappy.setTextureRect({34, 0, 17, 12});

    int bottom = app->window->getSize().y / 2 + 300;
    int top    = app->window->getSize().y / 2 - 300;

    target.clear();

    int floorCount = app->window->getSize().x / 48;

    float offsetNow  = dist % (app->window->getSize().x / floorCount);
    float offsetNext = dist % (app->window->getSize().x / floorCount) + 2;
    for (int i = 0; i <= floorCount + 1; i++) {
        floor.setPosition(48.f * float(i) - (offsetNow * ((dt - accum) / dt) + offsetNext * (accum / dt)),
                          bottom - 40);
        target.draw(floor);
    }

    for (auto pipe : pipes) {
        if (pipe.first > dist - app->window->getSize().x) {
            //captop.setPosition(app->window->getSize().x - (dist - pipe.first), pipe.second);
            int xPos = app->window->getSize().x - (dist - pipe.first);
            captop.setPosition(xPos * ((dt - accum) / dt) + (xPos + 2) * (accum / dt), pipe.second);
            target.draw(captop);
        }
    }

    flappy.setPosition(pX, pY * ((dt - accum) / dt) + (pY+pVel) * (accum / dt));
    flappy.setRotation(pRot * 57.3f);

    target.draw(flappy);
}

void SceneGame::setup_game(int seed) {
    app->texHolder.add_texture("flappy", "res/flappy_brown.png");
    app->texHolder.add_texture("captop", "res/captop_green.png");
    app->texHolder.add_texture("capbottom", "res/capbottom_green.png");
    app->texHolder.add_texture("pipe", "res/pipe_green.png");
    app->texHolder.add_texture("captop", "res/captop_green.png");
    app->texHolder.add_texture("background", "res/background_day.png");
}

void SceneGame::add_pipe(int seed) {
    std::cout << "adding pipe" << std::endl;
    pipes.push_back({dist+800, 500});
}

bool HandlerGame::handle(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Space) {
            static_cast<SceneGame*>(&app->get_scene("game"))->pVel = -5.f;
            return true;
        }
    }
    return false;
}
