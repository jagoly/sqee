#pragma once
#include <sqee/forward.hpp>

#include <sqee/handlers/handler.hpp>

namespace sq {

class HandlerClose : public Handler {
public:
    using Handler::Handler;
    bool handle(const sf::Event& _event);
};


class HandlerResize : public Handler {
public:
    using Handler::Handler;
    bool handle(const sf::Event& _event);
};

}
