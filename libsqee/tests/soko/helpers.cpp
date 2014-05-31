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
                   std::vector<std::vector<unsigned short int>>& objVec) {
    sf::Vector2i cOne;
    if      (dir == 0) cOne = {X, Y-1};
    else if (dir == 1) cOne = {X+1, Y};
    else if (dir == 2) cOne = {X, Y+1};
    else               cOne = {X-1, Y};

    if (cOne.x < 0 || cOne.y < 0 ||
        cOne.x >= size.x || cOne.y >= size.y)
        return false;

    unsigned short int oOne = objVec[cOne.y][cOne.x];
    if (oOne == 1 || oOne == 2) return false;

    sf::Vector2i cTwo;
    if (oOne == 3) {
        if      (dir == 0) cTwo = {X, Y-2};
        else if (dir == 1) cTwo = {X+2, Y};
        else if (dir == 2) cTwo = {X, Y+2};
        else               cTwo = {X-2, Y};

        if (cTwo.x == -1 || cTwo.y == -1 ||
            cTwo.x == size.x || cTwo.y == size.y)
            return false;

        unsigned short int oTwo = objVec[cTwo.y][cTwo.x];
        if (oTwo == 1 || oTwo == 3 || oTwo == 4) return false;
    }
    return true;
}
