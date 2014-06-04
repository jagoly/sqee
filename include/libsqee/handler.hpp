#pragma once

#include <SFML/Graphics.hpp>

#include <application.hpp>

namespace sq {

class Handler {
public:
    Handler(Application*);
    Application* app;

    virtual bool handle(sf::Event&);
};

}
