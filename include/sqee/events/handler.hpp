#pragma once
#include "forward.hpp"

#include <SFML/Window/Event.hpp>

namespace sq {

class Handler : NonCopyable {
public:
    using Ptr = unique_ptr<Handler>;

    Handler(Application& _app) : app(_app) {}

    virtual bool handle(const sf::Event& _event) {
        return false;
    }

protected:
    Application& app;
};

}
