#pragma once
#include <sqee/forward.hpp>

#include <sqee/handlers/Handler.hpp>

namespace sq {

class ConsoleHandler : public Handler {
public:
    using Handler::Handler;
    bool handle(const sf::Event& _event);

    void activate();
    void deactivate();
};

}
