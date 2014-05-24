#ifndef EVENTMANAGER_HPP
#define EVENTMANAGER_HPP

#include <SFML/Graphics.hpp>

namespace sq {

class Application;

class Handler {
public:
    Handler();
    Application* application;

    virtual bool handle(sf::Event&);
};

}

#endif // EVENTMANAGER_HPP
