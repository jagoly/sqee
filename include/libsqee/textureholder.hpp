#pragma once

#include <string>
#include <map>

#include <SFML/Graphics.hpp>

namespace sq {

class TextureHolder {
public:
    TextureHolder();

    sf::Texture& get_texture(std::string strId);

    void add_texture(std::string strId, std::string strPath);
    void remove_texture(std::string strId);
private:
    std::map<std::string, sf::Texture> textureMap;
};

}
