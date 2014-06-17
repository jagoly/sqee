#pragma once

#include <string>
#include <map>

#include <SFML/Graphics.hpp>

namespace sq {

class FontHolder {
public:
    FontHolder();

    sf::Font& get_font(std::string);

    void add_font(std::string, std::string);
    void remove_font(std::string);
private:
    std::map<std::string, sf::Font> fontMap;
};

}
