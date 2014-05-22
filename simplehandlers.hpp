#ifndef SIMPLEHANDLERS_HPP
#define SIMPLEHANDLERS_HPP

#include <SFML/Graphics.hpp>

#include <libsqee/libsqee.hpp>

namespace sqe {

class HandlerClose : public sq::EventHandler {
public:
    bool handle(sf::Event& event);
};

}


#endif // SIMPLEHANDLERS_HPP
