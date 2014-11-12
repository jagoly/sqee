#pragma once

#include <SFML/Window.hpp>

#include <defs.hpp>

namespace sq {

class Application;

class Handler {
public:
    typedef unique_ptr<Handler> Ptr;

    Handler(Application& _app)
        : app(_app) {}

    virtual bool handle(const sf::Event& event) {
        return false;
    }

protected:
    Application& app;
};

}

#include <app/application.hpp>
