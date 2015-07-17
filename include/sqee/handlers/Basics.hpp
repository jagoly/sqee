#pragma once
#include <sqee/forward.hpp>

#include <sqee/handlers/Handler.hpp>

namespace sq {

class BasicCloseHandler : public Handler {
public:
    using Handler::Handler;
    bool handle(const sf::Event& _event);
};


class BasicResizeHandler : public Handler {
public:
    using Handler::Handler;
    bool handle(const sf::Event& _event);
};

}
