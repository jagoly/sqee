#ifndef SIMPLEHANDLERS_HPP
#define SIMPLEHANDLERS_HPP

#include <SFML/Graphics.hpp>

#include <handler.hpp>

namespace sqe {

class HandlerClose : public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(sf::Event&);
};

class HandlerDebug : public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(sf::Event&);
};

class HandlerResize : public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(sf::Event&);
};

class HandlerFramelimit : public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(sf::Event&);
};

//class HandlerFreeze : public sq::Handler {      // Needs Git SFML
//    bool handle(sf::Event&);
//};

}

#endif // SIMPLEHANDLERS_HPP
