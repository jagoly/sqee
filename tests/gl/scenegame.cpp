#include <sqee/gl/gl.hpp>

#include "scenegame.hpp"

#include <iostream>
#include <fstream>

using namespace sqt;

SceneGame::SceneGame(sq::Application& _app) : sq::Scene(_app) {
    tickRate = 24;

    camera.init({0, 0, 2}, {0, 0, 0}, 4, 3, 1.17f, 0.1f, 100.f);
    camera.update_projMat();
    camera.update_viewMat();

    std::ifstream src;
    src.open("res/out.bin", std::ios::in | std::ios::binary | std::ios::ate);

    auto size = src.tellg();
    std::cout << size << std::endl;
    char* buf = new char[sizeof(float)];

    src.seekg(std::ios::beg);
    for (int i = 0; i < 6; i++) {
        src.read(buf, sizeof(float));
        std::cout << *(float*)buf << std::endl;
    }

    std::string s = "";

    std::cout << std::atoi(s.c_str()) << std::endl;



}

void SceneGame::update() {

}

void SceneGame::render(float) {

}

bool HandlerGame::handle(const sf::Event&) {
    return false;
}
