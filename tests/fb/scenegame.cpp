#include <random>

#include "scenegame.hpp"
#include "menus.hpp"
#include "helpers.hpp"

using namespace sqt;

SceneGame::SceneGame(sq::Application* _app) : sq::Scene(_app) {
    gen = std::minstd_rand(123456789);

    tickRate = 120;
    dt = 1/120.d;

    speed = 2;
    pVel = 0.f;
    dist = 0;

    pX = 100.f;
    pY = 320.f;

    flap = 0;

    fOffset = 0.f;
    fOffsetCount = 0;

    setup_game();
}

void SceneGame::update() {
    pRot = atan2(pVel, speed);
    dist += speed;

    pY += pVel;
    pVel += 0.08f;

    static float flapCount = 0.f;
    flapCount += abs(pVel);
    if (flapCount > 64) flapCount = 0;
    if      (flapCount > 48) flap = 1;
    else if (flapCount > 32) flap = 2;
    else if (flapCount > 16) flap = 1;
    else                     flap = 0;

    static uint newPipeCount = 0;
    newPipeCount += 1;
    if (newPipeCount == 180) {
        newPipeCount = 0;
        add_pipe();
    }
}

void SceneGame::render(sf::RenderTarget& target, float ft) {
    static sf::Sprite sprFlappy(app->texHolder.get_texture("flappy"));
    static sf::Sprite sprFloor(app->texHolder.get_texture("floor"));
    static sf::Sprite sprBg(app->texHolder.get_texture("background"));
    static sf::Sprite sprCaptop(app->texHolder.get_texture("captop"));
    static sf::Sprite sprCapbottom(app->texHolder.get_texture("capbottom"));
    static sf::Sprite sprPipe(app->texHolder.get_texture("pipe"));
    static bool first = true;
    if (first) {
        sprFlappy.setScale(4, 4);
        sprFloor.setScale(4, 4);
        sprBg.setScale(4, 4);
        sprCaptop.setScale(4, 4);
        sprCapbottom.setScale(4, 4);
        sprPipe.setScale(4, 4);

        sprFlappy.setTextureRect({17, 0, 17, 12});
        sprFlappy.setOrigin(8.5f, 6.f);

        first = false;
    }

    target.draw(sprBg);

    if      (flap == 1) sprFlappy.setTextureRect({17, 0, 17, 12});
    else if (flap == 0) sprFlappy.setTextureRect({0, 0, 17, 12});
    else if (flap == 2) sprFlappy.setTextureRect({34, 0, 17, 12});

    int offset = dist % (688);
    for (int i = 0; i <= 28; i++) {
        sprFloor.setPosition(float(48 * i) - interpolate(offset, offset + 2), 640);
        target.draw(sprFloor);
    }

    for (auto pipe : pipes) {
        if (pipe.first > dist - 800) {
            int xPos = 688 - dist + pipe.first;
            xPos = interpolate(xPos, xPos - 2);

            sprPipe.setPosition(xPos, 0);
            sprPipe.setScale(4, pipe.second - 172);
            sprCaptop.setPosition(xPos, pipe.second - 172);
            target.draw(sprPipe);
            target.draw(sprCaptop);

            sprPipe.setPosition(xPos, pipe.second + 172);
            sprPipe.setScale(4, 468 - pipe.second);
            sprCapbottom.setPosition(xPos, pipe.second + 120);
            target.draw(sprPipe);
            target.draw(sprCapbottom);
        }
    }

    sprFlappy.setPosition(pX, interpolate(pY, pY + pVel));
    sprFlappy.setRotation(pRot * 57.3f);

    target.draw(sprFlappy);
}

void SceneGame::setup_game() {
    app->texHolder.add_texture("flappy", "res/flappy_brown.png");
    app->texHolder.add_texture("captop", "res/captop_green.png");
    app->texHolder.add_texture("capbottom", "res/capbottom_green.png");
    app->texHolder.add_texture("pipe", "res/pipe_green.png");
    app->texHolder.add_texture("captop", "res/captop_green.png");
    app->texHolder.add_texture("background", "res/background_day.png");
}

void SceneGame::add_pipe() {
    static std::uniform_int_distribution<> ints(176, 464);
    static int previous = 320;

    int current = ints(gen) * 3;
    pipes.push_back({dist, (previous + current) / 4});
    previous = (previous + current) / 4;
}

bool HandlerGame::handle(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Space) {
            static_cast<SceneGame*>(&app->get_scene("game"))->pVel = -4.f;
            return true;
        }
    }
    return false;
}
