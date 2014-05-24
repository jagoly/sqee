#ifndef TEXTUREHOLDER_HPP
#define TEXTUREHOLDER_HPP

#include <string>
#include <map>

#include <SFML/Graphics.hpp>

namespace sq {

class TextureHolder {
public:
    TextureHolder();

    sf::Texture& get_texture(std::string);

    void add_texture(std::string, std::string);

    void remove_texture(std::string);
private:
    std::map<std::string, sf::Texture> textureMap;
};

}

#endif // TEXTUREHOLDER_HPP
