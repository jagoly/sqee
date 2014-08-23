#pragma once

#include <string>
#include <map>

#include <SFML/Graphics.hpp>

namespace sq {

class FontHolder {
public:
    FontHolder();

    sf::Font& get_font(std::string strId);

    void add_font(std::string strId, std::string strPath);
    void remove_font(std::string strId);
private:
    std::map<std::string, sf::Font> fontMap;
};

}
