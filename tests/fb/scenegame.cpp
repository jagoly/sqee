#include <random>
#include <ctime>

#include "scenegame.hpp"
#include "menus.hpp"
#include "helpers.hpp"

using namespace sqt;

SceneGame::SceneGame(sq::Application* _app) : sq::Scene(_app) {
    gen = std::minstd_rand(std::time(NULL));
    gen();

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
    pVel += 0.07f;

    static float flapCount = 0.f;
    flapCount += abs(pVel);
    if (flapCount > 64) flapCount = 0;
    if      (flapCount > 48) flap = 1;
    else if (flapCount > 32) flap = 2;
    else if (flapCount > 16) flap = 1;
    else                     flap = 0;

    static uint newPipeCount = 0;
    newPipeCount += 1;
    if (newPipeCount == 150) {
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
        sprFlappy.setScale(3, 3);
        sprFloor.setScale(3, 3);
        sprBg.setScale(3, 3);
        sprCaptop.setScale(3, 3);
        sprCapbottom.setScale(3, 3);
        sprPipe.setScale(3, 3);

        sprFlappy.setTextureRect({17, 0, 17, 12});
        sprFlappy.setOrigin(8.5f, 6.f);

        first = false;
    }

    target.draw(sprBg);

    if      (flap == 1) sprFlappy.setTextureRect({17, 0, 17, 12});
    else if (flap == 0) sprFlappy.setTextureRect({0, 0, 17, 12});
    else if (flap == 2) sprFlappy.setTextureRect({34, 0, 17, 12});

    int offset = dist % (612);
    for (int i = 0; i <= 34; i++) {
        sprFloor.setPosition(float(36 * i) - interpolate(offset, offset + 2), 666);
        target.draw(sprFloor);
    }

    for (auto pipe : pipes) {
        if (pipe.first > dist - 800) {
            int xPos = 612 - dist + pipe.first;
            xPos = interpolate(xPos, xPos - 2);

            sprPipe.setPosition(xPos, 0);
            sprPipe.setScale(3, pipe.second - 139);
            sprCaptop.setPosition(xPos, pipe.second - 139);
            target.draw(sprPipe);
            target.draw(sprCaptop);

            sprPipe.setPosition(xPos, pipe.second + 139);
            sprPipe.setScale(3, 527 - pipe.second);
            sprCapbottom.setPosition(xPos, pipe.second + 100);
            target.draw(sprPipe);
            target.draw(sprCapbottom);
        }
    }

    sprFlappy.setPosition(pX, interpolate(pY, pY + pVel));
    sprFlappy.setRotation(pRot * 57.3f);

    target.draw(sprFlappy);
}

void SceneGame::setup_game() {
    std::uniform_int_distribution<> dist3(0, 2);
    std::uniform_int_distribution<> dist2(0, 1);
    int val;

    val = dist3(gen);
    std::cout << val << std::endl;
    if      (val == 0) app->texHolder.add_texture("flappy", "res/flappy_brown.png");
    else if (val == 1) app->texHolder.add_texture("flappy", "res/flappy_blue.png");
    else if (val == 2) app->texHolder.add_texture("flappy", "res/flappy_red.png");

    if (dist2(gen)) {
        app->texHolder.add_texture("captop", "res/captop_green.png");
        app->texHolder.add_texture("capbottom", "res/capbottom_green.png");
        app->texHolder.add_texture("pipe", "res/pipe_green.png");
    } else {
        app->texHolder.add_texture("captop", "res/captop_red.png");
        app->texHolder.add_texture("capbottom", "res/capbottom_red.png");
        app->texHolder.add_texture("pipe", "res/pipe_red.png");
    }

    if (dist2(gen)) app->texHolder.add_texture("background", "res/background_day.png");
    else            app->texHolder.add_texture("background", "res/background_night.png");
}

void SceneGame::add_pipe() {
    static std::uniform_int_distribution<> ints(142, 524);
    static int previous = 336;

    int current = ints(gen) * 3;
    pipes.push_back({dist+600, (previous + current) / 4});
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
