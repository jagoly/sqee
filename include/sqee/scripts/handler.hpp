#pragma once
#include <sqee/forward.hpp>

#include <sqee/events/handler.hpp>

namespace sq {

class HandlerConsole : public Handler {
public:
    using Handler::Handler;
    bool handle(const sf::Event& _event);

    virtual void activate();
    virtual void deactivate();
};

}
