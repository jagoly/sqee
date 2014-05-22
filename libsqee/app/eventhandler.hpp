#ifndef EVENTMANAGER_HPP
#define EVENTMANAGER_HPP

#include <string>
#include <iostream>

#include <SFML/Graphics.hpp>

namespace sq {

class Application;

class EventHandler {
public:
    EventHandler();
    Application* application;

    virtual bool handle(sf::Event&);
};

}

#endif // EVENTMANAGER_HPP
