#pragma once

#include <vector>

#include <SFML/Graphics.hpp>

namespace sqt {

short int get_key_dir();

bool check_go(short int&, short int&, short int&, sf::Vector2i&,
              std::vector<std::vector<unsigned short int>>&);

}
