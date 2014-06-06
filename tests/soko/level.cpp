#include <iostream>
#include <fstream>

#include <libsqee/extra/helpers.hpp>

#include "level.hpp"

using namespace sqt;

Level::Level(std::string filePath) {
    Json::Value root = sqe::load_json_file(filePath);

    width = root["width"].asInt();
    height = root["height"].asInt();

    unsigned short int x = 0;
    unsigned short int y = 0;
    std::vector<unsigned short int> vec;

    for (auto val : root["textures"]) {
        vec.push_back(val.asInt());
        if (x < width - 1) {
            x++;
        } else {
            textures.push_back(vec);
            vec.clear();
            x = 0;
            y++;
        }
    }

    y = 0;
    for (auto val : root["objects"]) {
        if (val.asInt() == 5) {
            pX = x;
            pY = y;
            vec.push_back(0);
        } else {
            vec.push_back(val.asInt());
        }
        if (x < width - 1) {
            x++;
        } else {
            objects.push_back(vec);
            vec.clear();
            x = 0;
            y++;
        }
    }

    x = 0;
    for (auto val : root["paths"]) {
        paths.push_back(std::pair<unsigned short int, std::string>(x++, val.asString()));
    }

    // Debugging Info
    std::cout << "Loading level \"" << root["name"].asString() << "\"\nfrom file \"" << filePath << "\"" << std::endl;

    std::cout << "Start: (" << pX << ", " << pY << ")\n" << std::endl;

    std::cout << "Textures:" << std::endl;
    for (auto row : textures) {
        for (auto col : row)
            std::cout << col;
        std::cout << std::endl;
    }

    std::cout << std::endl;
    std::cout << "Objects:" << std::endl;
    for (auto row : objects) {
        for (auto col : row)
            std::cout << col;
        std::cout << std::endl;
    }

    std::cout << std::endl;
    std::cout << "Textures:" << std::endl;
    for (auto path : paths) {
        std::cout << path.first << ": " << path.second;
        std::cout << std::endl;
    }
}
