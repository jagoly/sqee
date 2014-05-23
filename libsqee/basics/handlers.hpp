#ifndef SIMPLEHANDLERS_HPP
#define SIMPLEHANDLERS_HPP

#include <SFML/Graphics.hpp>

#include <libsqee/libsqee.hpp>

namespace sqe {

class HandlerClose : public sq::Handler {
public:
    bool handle(sf::Event& event);
};

class HandlerDebug : public sq::Handler {
public:
    bool handle(sf::Event& event);
};

}

#endif // SIMPLEHANDLERS_HPP
