#include <iostream>

#include <textureholder.hpp>

using namespace sq;

TextureHolder::TextureHolder() {
}

sf::Texture& TextureHolder::get_texture(std::string strId) {
    return textureMap[strId];
}

void TextureHolder::add_texture(std::string strId, std::string strPath) {
    textureMap.emplace("strId", sf::Texture());
    textureMap[strId].loadFromFile(strPath);
}