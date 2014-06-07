#pragma once

#include <SFML/Graphics.hpp>

namespace sq {

class Application;

class Handler {
public:
    Handler(Application*);
    Application* app;

    virtual bool handle(sf::Event&);
};

}
