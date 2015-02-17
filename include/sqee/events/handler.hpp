#pragma once
#include "forward.hpp"

#include <memory>

#include <SFML/Window/Event.hpp>

namespace sq {

class Handler : NonCopyable {
public:
    typedef std::unique_ptr<Handler> Ptr;

    Handler(Application& _app) : app(_app) {}

    virtual bool handle(const sf::Event& _event) {
        return false;
    }

protected:
    Application& app;
};

}
