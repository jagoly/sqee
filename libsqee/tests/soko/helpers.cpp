#include <SFML/Graphics.hpp>

#include <libsqee/tests/soko/helpers.hpp>

short int sqt::get_key_dir() {
   if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
       return 0;
   } else
   if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
       return 1;
   } else
   if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
       return 2;
   } else
   if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
       return 3;
   }
   return -1;
}

bool sqt::check_go(short int& dir, short int& X, short int& Y, sf::Vector2i& size,
                   std::vector<std::vector<unsigned short int>>& objs) {


}
