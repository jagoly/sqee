#include <random>
#include <ctime>
#include <algorithm>

#include "scenegame.hpp"
#include "menus.hpp"
#include "helpers.hpp"

namespace sqt {

SceneGame::SceneGame(sq::Application* _app) : sq::Scene(_app) {
    gen = std::minstd_rand(std::time(NULL));
    gen();

    crashed = false;

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

    score = 0;

    setup_game();
}

void SceneGame::update() {
    if (crashed) {
        update_crashed();
        return;
    }

    Rect bBox(pX-22.5f, pY-15.f, 42.f, 33.f);
    bBox.rotate_fresh(pRot, pX, pY);

    pRot = atan2(pVel, speed);
    dist += speed;

    pY += pVel;
    pVel += 0.07f;

    for (auto pipe : pipes) {
        int xPos = 612 - dist + pipe.first;
        if (xPos > 0 && xPos < 180) {
            if (oRect_intersects_rect(bBox,
                    {float(xPos), 0, 78, float(pipe.second-100)}) ||
                oRect_intersects_rect(bBox,
                    {float(xPos), float(pipe.second+100), 78, float(605-pipe.second)})) {
                crashed = true;
                pVel = std::max<float>(pVel, 0.f);
                app->soundManager.play_sound("hit");
                return;
            }
        }
        if (pY > 650.f) {
            crashed = true;
            app->soundManager.play_sound("hit");
            return;
        }
    }

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
    } else
    if (newPipeCount == 114 && pipes.size() > 3) {
        score += 1;
        app->soundManager.play_sound("point");
    }
}

void SceneGame::render(sf::RenderTarget& target, float) {
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

    float offset = dist % (612);
    if (!crashed) offset = interpolate(offset, offset + 2.f);
    for (int i = 0; i <= 34; i++) {
        sprFloor.setPosition(float(36 * i) - offset, 666);
        target.draw(sprFloor);
    }

    for (auto pipe : pipes) {
        if (pipe.first > dist - 800) {
            int xPos = 612 - dist + pipe.first;
            if (!crashed) xPos = interpolate(xPos, xPos - 2);

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

    sprFlappy.setPosition(pX, pY);
    sprFlappy.setRotation(pRot * 57.3f);

    target.draw(sprFlappy);
}

void SceneGame::setup_game() {
    std::uniform_int_distribution<> dist3(0, 2);
    std::uniform_int_distribution<> dist2(0, 1);
    int val;

    val = dist3(gen);
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

void SceneGame::update_crashed() {
    if (pY >= 650.f) {
        pY = 650.f;
        pRot = 1.57f;
        return;
    }
    static float rSpeed = speed;
    pY += pVel;
    pVel += 0.08f;

    pRot = atan2(pVel, rSpeed);

    rSpeed = std::max<float>(rSpeed - 0.02f, 0);
}

bool HandlerGame::handle(sf::Event& event) {
    if (!static_cast<SceneGame*>(&app->get_scene("game"))->crashed) {
        if ((SQ_KEYPRESS(Space) | SQ_LEFTMPRESS) &&
            static_cast<SceneGame*>(&app->get_scene("game"))->pY > 20) {
            static_cast<SceneGame*>(&app->get_scene("game"))->pVel = -4.f;
            app->soundManager.play_sound("wing");
            return true;
        }
    }
    return false;
}

}
