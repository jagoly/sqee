#include <iostream>
#include <fstream>

#include <jsoncpp/json/json.h>

#include "level.hpp"

using namespace sqt;

Level::Level(std::string filePath) {
    std::ifstream src(filePath);

    Json::Value root;
    Json::Reader reader;
    if (!reader.parse(src, root)) {
        std::cout << "level load failed:" << std::endl << reader.getFormattedErrorMessages();
    }
    src.close();

    width = root["width"].asInt();
    height = root["height"].asInt();

    unsigned short int i = 0;
    std::vector<unsigned short int> vec;

    for (auto val : root["textures"]) {
        i++;
        if (i < width) {
            vec.push_back(val.asInt());
        } else {
            vec.push_back(val.asInt());
            textures.push_back(vec);
            vec.clear();
            i = 0;
        }
    }

    i = 0;
    vec.clear();
    for (auto val : root["objects"]) {
        i++;
        if (i < width) {
            vec.push_back(val.asInt());
        } else {
            vec.push_back(val.asInt());
            objects.push_back(vec);
            vec.clear();
            i = 0;
        }
    }

    i = 0;
    for (auto val : root["paths"]) {
        paths.push_back(std::pair<unsigned short int, std::string>(i++, val.asString()));
    }

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
