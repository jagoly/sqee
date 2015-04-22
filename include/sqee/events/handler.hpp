#pragma once
#include <sqee/forward.hpp>

#include <SFML/Window/Event.hpp>

namespace sq {

class Handler : NonCopyable {
public:
    Handler(Application& _app) : app(_app) {}
    virtual bool handle(const sf::Event& _event) = 0;

protected:
    Application& app;
};

}
