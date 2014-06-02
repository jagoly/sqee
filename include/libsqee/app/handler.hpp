#ifndef EVENTMANAGER_HPP
#define EVENTMANAGER_HPP

#include <SFML/Graphics.hpp>

#include <app/application.hpp>

namespace sq {

class Handler {
public:
    Handler();
    Application* application;

    virtual bool handle(sf::Event&);
};

}

#endif // EVENTMANAGER_HPP
