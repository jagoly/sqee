#pragma once
#include <sqee/forward.hpp>

#include <sqee/events/handler.hpp>

namespace sqt {

class HandlerMain : public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(const sf::Event& event);
};

}
