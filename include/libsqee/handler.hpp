#pragma once

#include <SFML/Graphics.hpp>

#include <memory>

namespace sq {

class Application;

class Handler {
public:
    Handler(Application*);

    virtual bool handle(sf::Event&);

protected:
    Application* app;
};

typedef std::shared_ptr<Handler> HandlerPtr;

}
